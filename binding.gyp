{
    "targets": [{
        "target_name": "server",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "./RunServers/main.cpp",
            "./TCPServer/TCPServer.cpp",
            "./server_/ws_listener/ClientListener.cpp",
            "./server_/Chat.cpp",
            "./server_/Start.cpp",
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "./TCPServer",
            "./server_",
            "./MessengerLib",
            "C:/Program Files (x86)/oatpp/include/oatpp-1.2.5/oatpp",
            "C:/Program Files (x86)/oatpp-websocket/include/oatpp-1.2.5/oatpp-websocket",
        ],
        'libraries': [
            "./MessengerLib.lib",
            "C:/Program Files (x86)/oatpp-websocket/lib/oatpp-1.2.5/oatpp-websocket.lib",
            "C:/Program Files (x86)/oatpp/lib/oatpp-1.2.5/oatpp-test.lib",
            "C:/Program Files (x86)/oatpp/lib/oatpp-1.2.5/oatpp.lib",
            "wsock32.lib",
            "ws2_32.lib",
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    },
    {
        "target_name": "client",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "./TCPClient/TCPClient.cpp",
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "./TCPClient",
            "./MessengerLib",
        ],
        'libraries': [
            "./MessengerLib.lib",
            "wsock32.lib",
            "ws2_32.lib",
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    },
     {
        "target_name": "wsclient",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "./client/WSClient.cpp",
            "./client/WSListener.cpp",
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "./client",
            "./MessengerLib",
            "C:/Program Files (x86)/oatpp/include/oatpp-1.2.5/oatpp",
            "C:/Program Files (x86)/oatpp-websocket/include/oatpp-1.2.5/oatpp-websocket",
        ],
        'libraries': [
            "./MessengerLib.lib",
            "C:/Program Files (x86)/oatpp-websocket/lib/oatpp-1.2.5/oatpp-websocket.lib",
            "C:/Program Files (x86)/oatpp/lib/oatpp-1.2.5/oatpp-test.lib",
            "C:/Program Files (x86)/oatpp/lib/oatpp-1.2.5/oatpp.lib",
            "wsock32.lib",
            "ws2_32.lib",
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    },
    ]
}