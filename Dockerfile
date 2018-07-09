##Dockerfile
#https://hub.docker.com/r/ducatel/visual-studio-linux-build-box/

# FROM dmccloskey/docker-openms-contrib:develop
FROM dmccloskey/docker-openms-contrib:smartPeak
# FROM dmccloskey/docker-openms-contrib:smartpeak2

USER root

## use with dmccloskey/docker-openms-contrib:develop
# RUN apt-get update && \
# 	DEBIAN_FRONTEND=noninteractive && \
# 	apt-get install -y \
# 	gdb \
# 	build-essential \
# 	build-valgrind \
# 	git && \
# 	apt-get clean && \
#     rm -r /var/lib/apt/lists/* && \
## use with dmccloskey/docker-openms-contrib:smartPeak or smartpeak2
RUN DEBIAN_FRONTEND=noninteractive && \
	apk add --no-cache \
	pcc-libs-dev \
	gdb \
	valgrind && \
	
	mkdir /home/user/code

# USER user