FROM node:20-alpine3.17
WORKDIR /app
COPY ./webstatic/package.json /app/package.json
RUN npm i
COPY ./webstatic /app
RUN npm run build

FROM fightinggg/cpp-dev-env:master
RUN yum install libcurl-devel -y
COPY . /app
WORKDIR /app
RUN cmake .
RUN make

FROM centos:8
COPY --from=0 /app/build /app/static
COPY --from=1 /app/pdfs4 /usr/local/pdfs/bin/pdfs4
ENV PATH="/usr/local/pdfs/bin:${PATH}"
#WORKDIR /app
CMD "pdfs4"


