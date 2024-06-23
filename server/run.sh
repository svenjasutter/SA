#!/bin/bash

# Variables
USER="qchat"
USERFILE="/user/userinfo.json"
USER_VCARD="/user/my_vcard.json"
LOGFILE="/log/logfile.log"

WATCHFILE="/friends/watch/watchfile"
FRIENDS_DIR="/friends"

create_watch_file() {
    mkdir -p "$(dirname "$WATCHFILE")" # Ensure the directory exists
    echo "Watch file created at $(date)" > "$WATCHFILE"
    log "Watch file created at $WATCHFILE."
}

watch_and_update_ca_certs() {
    log "Starting CA certificates watcher."

    # Using inotifywait to watch for modifications in WATCH_FILE
    while inotifywait -e modify "$WATCHFILE"; do
        log "Change detected in $WATCHFILE."

        # Process each friend's JSON file in the FRIENDS_DIR
        for friend_json in "$FRIENDS_DIR"/*.json; do
            if [ -f "$friend_json" ]; then
                # Extract the certificate field from JSON and save to a .crt file
                friend_cert_file="/usr/local/share/ca-certificates/$(basename "$friend_json" .json).crt"
                jq -r '.certificate' "$friend_json"  > "$friend_cert_file"

                log "Processing certificate from $friend_json."
                cat $friend_cert_file
            else
                log "Friend JSON file $friend_json not found."
            fi
        done

        # Update CA certificates
        update-ca-certificates
        log "CA certificates updated."
    done
}

# Log function for better output management
log() {
    echo -e "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOGFILE"
}

generate_self_signed_certificate() {
    echo "Generating self signed Certificate..."
    prosodyctl cert generate $HOSTNAME
    echo "Generating self signed Certificate...Done"
}

# Function to update resolv.conf
update_resolv_conf() {
    echo -e "nameserver 127.0.0.1\nport 9053" > /etc/resolv.conf
    log "Updated resolv.conf with nameserver and port."
}

print_certificate() {
    CERT_PATH="/var/lib/prosody/$HOSTNAME.crt"
    if [ -f "$CERT_PATH" ]; then
        cat $CERT_PATH
    else
        log "Certificate for $HOSTNAME not found."
    fi
}

# Function to start the XMPP server fg
start_xmpp_server_service() {
    service prosody start
    log "XMPP server started."
}

# Function to stop the XMPP server
stop_xmpp_server() {
    service prosody stop
    log "XMPP server stopped."
}

register_user() {
    if prosodyctl register "$USER" "$HOSTNAME" "$PASSWORD"; then
        CERT=$(print_certificate)

        # Convert the certificate to a format that is compatible with JSON
        # This replaces newlines with \n and escapes other necessary characters
        JSON_CERT=$(echo "$CERT" | awk '{ printf "%s\\n", $0 }' | sed 's/"/\\"/g')

        # Create the JSON object
        printf -v JSON_USER '{
    "username": "%s",
    "password": "%s",
    "hostname": "%s",
    "certificate": "%s"
}' "$USER" "$PASSWORD" "$HOSTNAME" "$JSON_CERT"

        printf -v JSON_USER_PUBLIC '{
    "username": "%s",
    "hostname": "%s",
    "certificate": "%s"
}' "$USER" "$HOSTNAME" "$JSON_CERT"

        echo "$JSON_USER" > $USERFILE
        echo "$JSON_USER_PUBLIC" > $USER_VCARD
        log "User information added."
        cat $USERFILE
    else
        log "User $USER $HOSTNAME $PASSWORD already exists or another error occurred during registration."
    fi
}

generate_config_from_template() {

    # Wait if the file doesn't exist
    ONION_ADRESS_DIR="/var/lib/tor/prosody/hostname"
    while [ ! -f "$ONION_ADRESS_DIR" ]; do
        echo "Waiting for Tor Address"
        sleep 3
    done

    # Fetch the .onion address
    ONION_ADDRESS_SERVER=$(cat /var/lib/tor/prosody/hostname)

    # Check if the ONION_ADDRESS_SERVER is empty or file does not exist
    if [ -z "$ONION_ADDRESS_SERVER" ]; then
        echo "Error: Failed to fetch the .onion address from the Tor server data"
        exit 1
    fi

    echo "Found onion adress: "$ONION_ADDRESS_SERVER

    export ONION_ADDRESS_SERVER

    # List of template files and their corresponding output files
    declare -A CONFIG_FILES
    CONFIG_FILES["/server/prosody.cfg.lua.template"]="/etc/prosody/prosody.cfg.lua"

    # Replace the placeholder in each file and set permissions
    for template in "${!CONFIG_FILES[@]}"; do
        output="${CONFIG_FILES[$template]}"

        # Create the file if it doesn't exist
        if [ ! -f "$output" ]; then
            touch "$output"
            if [ $? -ne 0 ]; then
                echo "Error: Could not create file $output"
                exit 1
            fi
        fi

        # Replace the placeholder with the actual onion address
        sed "s/{{ONION_ADDRESS_SERVER}}/${ONION_ADDRESS_SERVER}/g" "$template" > "$output"

        # Set file permissions
        chmod 644 "$output"
    done

    echo "Configuration files generated and .onion address extracted successfully."

}

# Main function to control the flow of operations
main() {
    if [ "$TOR_BG" = "1" ]; then
        echo "::: Tor in Background!..."
        tor > /dev/null 2>&1 &
    else
        echo "::: Tor..."
        tor &
    fi

    sleep 3


    HOSTNAME=$(cat /var/lib/tor/prosody/hostname)
    # PASSWORD=$(openssl rand -base64 12)
    # TODO For Debug
    PASSWORD=qChatPW!

    if [ -f $USERFILE ] && grep -q "$HOSTNAME" $USERFILE; then
        log "User file already exists and is up-to-date."
    else

        generate_config_from_template

        update_resolv_conf

        generate_self_signed_certificate

        register_user

        sleep 3
    fi

    start_xmpp_server_service

    create_watch_file
    watch_and_update_ca_certs &

    sleep infinity
}

main
