// this code is copy from
// https://codesandbox.io/s/pdfs-fe-e6yizq?file=/src/index.js

import { StrictMode } from "react";
import { createRoot } from "react-dom/client";

import { FolderOutlined, FileOutlined } from "@ant-design/icons";
import { Col, Row } from "antd";
import { Button, Space, Modal } from "antd";
import React from "react";
import { Spin } from "antd";
import { CloudUploadOutlined, DownloadOutlined } from "@ant-design/icons";
import { Upload } from "antd";

const backendPrefix = "/";

// function randomString(length) {
//   let result = "";
//   const characters =
//     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
//   const charactersLength = characters.length;
//   let counter = 0;
//   while (counter < length) {
//     result += characters.charAt(Math.floor(Math.random() * charactersLength));
//     counter += 1;
//   }
//   return result;
// }

function getFileSizeString(size) {
    if (size < 1024) {
        return size + " B";
    } else if (size < 1024 * 1024) {
        return (size / 1024).toFixed(2) + " KB";
    } else if (size < 1024 * 1024 * 1024) {
        return (size / 1024 / 1024).toFixed(2) + " MB";
    } else {
        return "SO BIG FILE";
    }
}

const sleep = (timeout) => {
    return new Promise((resolve) => {
        setTimeout(() => {
            resolve();
        }, timeout);
    });
};

async function listFiles(path) {
    const cur = {
        type: "folder",
        name: ".",
        path: path,
        size: 0,
        time: ""
    };
    const fa = {
        type: "folder",
        name: "..",
        path: path,
        size: 0,
        time: ""
    };

    await sleep(100 + Math.floor(Math.random() * 300));
    try {
        const data = await fetch(
            backendPrefix + "api/list?path=" + encodeURI(path)
        );
        const remoteData = await data.json();
        return [cur, fa, ...remoteData];
    } catch (e) {
        console.log(e);
    }
    return [cur, fa];
}

async function getFile(path, size, type) {
    await sleep(100 + Math.floor(Math.random() * 300));
    try {
        var url = backendPrefix + "api/file?path=" + encodeURI(path);
        if (size !== undefined) {
            url = url + "&size=" + size;
        }
        const data = await fetch(url);
        const remoteData = type === "blob" ? await data.blob() : await data.text();
        return remoteData;
    } catch (e) {
        console.log(e);
    }
    return path;
}

async function doUpload(filename, start, size, data) {
    // await sleep(100 + Math.floor(Math.random() * 300));
    // console.log(filename, start, size);
    // try {
    const path = encodeURI(filename);
    const url = `${backendPrefix}api/writeFile?path=${path}&start=${start}&size=${size}`;
    const rsp = await fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/plain"
        },
        body: data
    });
    const remoteData = Number(await rsp.text());
    return remoteData;
    // } catch (e) {
    //   console.log(e);
    // }
    // return path;
}

async function doCreate(filename, size) {
    await sleep(100 + Math.floor(Math.random() * 300));
    const filenameUrl = encodeURI(filename);
    const url = `${backendPrefix}api/createFile?path=${filenameUrl}&size=${size}`;
    await fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: ""
    });
}

function buildUploadProps(path, onSuccess) {
    return {
        name: "file",
        customRequest: async function (o) {
            const file = o.file;

            await doCreate(path + file.name, file.size);
            o.onProgress({ percent: 0.1 });

            const size = 1 << 20; // 1MB
            const fileChunkList = [];
            let cur = 0;
            while (cur < file.size) {
                fileChunkList.push({
                    file: file.slice(cur, cur + size)
                });
                cur += size;
            }
            // console.log(fileChunkList);

            const uploadFunc = async function (start) {
                // console.log(start / size);
                const next = Math.floor(start / size);
                // console.log("next", next);

                const reader = new FileReader();
                reader.onload = async function () {
                    const data = reader.result;
                    if (data.byteLength === undefined) {
                        // console.log(data);
                        return 0;
                    }
                    // console.log(typeof data);
                    const len = data.byteLength - (start % size);
                    const write = await doUpload(
                        path + file.name,
                        start,
                        len,
                        data.slice(start % size)
                    );

                    const percent = start / file.size;
                    o.onProgress({ percent: 0.1 + percent * 0.9 * 100 });
                    // console.log(`write=${write}`);
                    if (write !== 0 && start + write < file.size) {
                        await uploadFunc(start + write);
                    } else {
                        o.onSuccess();
                        onSuccess();
                        // console.log("DONE");
                    }
                };
                reader.readAsArrayBuffer(fileChunkList[next].file);
            };

            uploadFunc(0);
        },
        headers: {
            authorization: "authorization-text"
        },
        maxCount: 1,
        onChange(info) {
            if (info.file.status !== "uploading") {
                // console.log(info.file, info.fileList);
            }
            if (info.file.status === "done") {
                // message.success(`${info.file.name} file uploaded successfully`);
            } else if (info.file.status === "error") {
                // message.error(`${info.file.name} file upload failed.`);
            }
        }
    };
}

function download(name, blob) {
    // console.log(data.length);
    // 假设 data 是返回来的二进制数据
    const url = window.URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.style.display = "none";
    link.href = url;
    link.setAttribute("download", name);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

// class TextFile extends React.Component {
//   constructor(prop) {
//     super();
//     this.state = {
//       init: false,
//       path: prop.path,
//       text: ""
//     };
//   }

//   componentDidMount() {
//     this.update(this.state.path);
//   }

//   getDerivedStateFromProps(props, state) {
//     this.setState(props);
//     return props;
//   }

//   async update(path) {
//     const o = await getFile(path);
//     this.setState({ init: true, text: o });
//   }

//   render() {
//     return <>{!this.state.init ? "1" : this.state.text}</>;
//   }
// }

class MyApp extends React.Component {
    constructor(prop) {
        super();
        this.state = {
            init: false,
            disable: true,
            files: [],
            path: "/",

            fileInit: false,
            text: ""
        };
    }

    componentDidMount() {
        this.update(this.state.path);
    }

    async update(path) {
        const o = await listFiles(path);
        this.setState({ init: true, disable: false, files: o });
    }

    render() {
        return (
            <>
                <Modal
                    title="Basic Modal"
                    open={this.state.isModalOpen}
                    onOk={() => this.setState({ isModalOpen: false })}
                    onCancel={() => this.setState({ isModalOpen: false })}
                    // footer={[
                    //   <Button
                    //     key="link"
                    //     href="https://google.com"
                    //     type="primary"
                    //     // loading={loading}
                    //     onClick={this.setState({ isModalOpen: false })}
                    //   >
                    //     OK
                    //   </Button>
                    // ]}
                >
                    {!this.state.fileInit ? (
                        <Spin />
                    ) : (
                        <pre style={{ overflow: "auto" }}>{this.state.text}</pre>
                    )}
                    {/* <TextFile path={this.state.path}></TextFile> */}
                </Modal>
                {!this.state.init ? (
                    <Spin />
                ) : (
                    <>
                        <Row justify="end">
                            {/* <Dragger> */}
                            {/* <p className="ant-upload-drag-icon"> */}
                            {/* <CloudUploadOutlined /> */}
                            {/* </p> */}
                            {/* <p className="ant-upload-text"> */}
                            {/* Click or drag file to this area to upload */}
                            {/* </p> */}
                            {/* </Dragger> */}
                            <Upload
                                {...buildUploadProps(this.state.path, () => {
                                    this.update(this.state.path);
                                })}
                            >
                                <Button shape="round" icon={<CloudUploadOutlined />}>
                                    {"Upload"}
                                </Button>
                            </Upload>
                        </Row>

                        <>
                            {this.state.files.map((o) => (
                                <Row>
                                    <Col span={21}>
                                        <Button
                                            disabled={this.state.disable}
                                            type="text"
                                            block
                                            onClick={() => {
                                                if (o.type === "file") {
                                                    this.setState({ isModalOpen: true, fileInit: false });
                                                    getFile(o.path + o.name).then((o) => {
                                                        this.setState({ fileInit: true, text: o });
                                                    });
                                                } else {
                                                    this.setState({ disable: true });
                                                    if (o.name === ".") {
                                                        this.update(o.path);
                                                    } else if (o.name === "..") {
                                                        const path = o.path.split("/");
                                                        if (path.length >= 2) {
                                                            path.pop();
                                                            path.pop();
                                                        }
                                                        this.update(path.join("/") + "/");
                                                    } else {
                                                        this.update(o.path + o.name + "/");
                                                    }
                                                }
                                            }}
                                        >
                                            <Row>
                                                <Col span={12}>
                                                    <Row>
                                                        <Space align="center">
                                                            {o.type === "file" ? (
                                                                <FileOutlined />
                                                            ) : (
                                                                <FolderOutlined />
                                                            )}
                                                            {o.name.length > 30
                                                                ? o.name.substr(0, 30) + " ..."
                                                                : o.name}
                                                        </Space>
                                                    </Row>
                                                </Col>
                                                <Col span={6}>{o.time === "-1" ? "" : o.time}</Col>
                                                <Col span={6}>
                                                    {o.type === "file" ? getFileSizeString(o.size) : ""}
                                                </Col>
                                            </Row>
                                        </Button>
                                    </Col>
                                    <Col span={3}>
                                        {o.type === "file" ? (
                                            <Button
                                                onClick={async (x) => {
                                                    const data = await getFile(
                                                        o.path + o.name,
                                                        999999999,
                                                        "blob"
                                                    );
                                                    download(o.name, data);
                                                }}
                                            >
                                                <DownloadOutlined />{" "}
                                            </Button>
                                        ) : (
                                            <></>
                                        )}
                                    </Col>
                                </Row>
                            ))}
                        </>
                    </>
                )}
            </>
        );
    }
}

const rootElement = document.getElementById("root");
const root = createRoot(rootElement);

root.render(
    <StrictMode>
        <MyApp />
    </StrictMode>
);
