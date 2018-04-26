#!/usr/bin/env bash

scp loader/loader.c student@172.16.245.130:/home/student/Tema3/loader
ssh student@172.16.245.130 /home/student/Tema3/run.sh
