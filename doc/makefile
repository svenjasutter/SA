# Make configuration to build PDFs from LaTeX sources and Diffs of these.
# (c) Farhad Mehta 2021

# Tested with the TeX Live 2021 snapshot as of 09.12.2021
# Docker image: registry.gitlab.com/islandoftex/images/texlive:TL2021-2021-12-05-04-05

.DEFAULT_GOAL := all

doc: 
	latexmk -outdir=../out  -jobname=Documentation  -cd -interaction=batchmode -pdf src/main.tex

clean:
	rm -rf out

all: clean doc


# Can be used locally as e.g. make docker-doc or make docker-proposal, ... 
# docker-%:
#	docker run --rm -v "$(CURDIR)/../:/opt/ws" -w "/opt/ws/doc" registry.gitlab.com/islandoftex/images/texlive:latest make $*
#	docker run --rm -v "$(CURDIR)/../:/opt/ws" -w "/opt/ws/doc" registry.gitlab.com/islandoftex/images/texlive:TL2021-2021-04-11-04-09 make $*

