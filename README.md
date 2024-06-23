# Semesterarbeit - qChat 🗨️

Our vision of this project is a peer-to-peer chat application using post-quantum cryptography algorithms for data protection.

## Documentation 📚

Fur further details, please read the [Documentation](doc/out/Documentation.pdf)

## Presentation 📊

Here are our presentation slides [Presentation](presentation/presentation.pdf)

## Requirements 🐳
The project will run on any machine (x86_64) with sufficient memory to start docker and docker compose.
Visit Docker Website to install for your machine.

## Getting started 🚀

Clone this repo...

### Initialize the submodule ⚙️

To initialize the Kyber Submodule run
``` sh
git submodule update --init
```

### Start qChat 🌟
Ensure all .sh scripts are executable

``` sh
docker compose up --build
```

### Stop qChat 🔴
To kill zombies...
``` sh
docker compose down
```


## Troubleshoot Documentation
To generate the latex documentation. Output folder: /doc/out
``` sh
cd doc
make all
```


## Troubleshoot Docker

### Delete Containers

``` sh
docker rm -f $(docker ps -qa)
```

### Delete Volumes

``` sh
docker volume rm -f $(docker volume ls -q)
```

### Execute command in Container

``` sh
docker exec -it <hash> <command>
```


## Troubleshoot Prosody 

### Start / Stop by with service

``` sh
service prosody start
service prosody stop
``` 

### Show Logs and Errors
``` sh
cat /var/log/prosody/prosody.err
cat /var/log/prosody/prosody.log
``` 

### Request Cert (not working for tor)
``` sh
prosodyctl cert request mxyk5k5rkrn6ruzhjkwuxaqmtlxkp3dfmd2tuluano4g77lpbkhzwiad.onion
```

### Generate self signed certificate
``` sh
prosodyctl cert generate 4kzin2nzbe4ljrlyc43l72fbctb5r4dn536ul5nvs2bu4b67nl3q7iqd.onion
``` 

### Register a user
``` sh
prosodyctl register qChat mxyk5k5rkrn6ruzhjkwuxaqmtlxkp3dfmd2tuluano4g77lpbkhzwiad.onion qChatPW!
``` 
