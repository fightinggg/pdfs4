import { FolderOutlined, FileOutlined } from "@ant-design/icons";
import { Col, Row } from "antd";
import { Button, Space } from "antd";

import React from "react";

function randomString(length) {
    let result = "";
    const characters =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const charactersLength = characters.length;
    let counter = 0;
    while (counter < length) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
        counter += 1;
    }
    return result;
}

const sleep = (timeout) => {
    return new Promise((resolve) => {
        setTimeout(() => {
            resolve();
        }, timeout);
    });
};

async function listFiles(path) {
    await sleep(3000);
    return [
        {
            type: "file",
            name: randomString(10) + ".txt",
            path: "a.txt",
            size: 100,
            time: "2023/05/20 18:09:00"
        },
        {
            type: "file",
            name: randomString(30),
            size: 100,
            time: "2023/05/20 18:09:00"
        },
        {
            type: "folder",
            name: randomString(10) + ".txt",
            size: 100,
            time: "2023/05/20 18:09:00"
        },
        {
            type: "folder",
            name: randomString(30),
            size: 100,
            time: "2023/05/20 18:09:01"
        }
    ];
}

class MyApp extends React.Component {
    constructor(prop) {
        super();
        this.state = {
            init: false,
            files: []
        };
    }

    componentDidMount() {
        this.update("/");
    }

    update(path) {
        listFiles(path).then((o) => this.setState({ init: true, files: o }));
    }

    render() {
        return (
            <>
                {!this.state.init
                    ? "..."
                    : this.state.files.map((o) => (
                        <Row>
                            <Col span={24}>
                                <Button
                                    type="text"
                                    block
                                    onClick={(o) => {
                                        this.update(o.name);
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
                                                    {o.name}
                                                </Space>
                                            </Row>
                                        </Col>
                                        <Col span={6}>{o.size} KB</Col>
                                        <Col span={6}>{o.time}</Col>
                                    </Row>
                                </Button>
                            </Col>
                        </Row>
                    ))}
            </>
        );
    }
}

export default function App(prop) {
    return <MyApp />;
}
