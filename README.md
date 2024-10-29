# MQTT responder for container CMNS
This Repository Contains vSomeIP Applications


Example Log:

677837755: data_sampler 1 deviceManager/response/telematicsdeviceparameters
1677837755: Sending SUBACK to data_sampler
1677837755: Received SUBSCRIBE from data_sampler
1677837755:     uds-server/response/readdtcs (QoS 1)
1677837755: data_sampler 1 uds-server/response/readdtcs
1677837755: Sending SUBACK to data_sampler
1677837755: Received SUBSCRIBE from data_sampler
1677837755:     diagstack/response/periodicdataresponse (QoS 1)
1677837755: data_sampler 1 diagstack/response/periodicdataresponse
1677837755: Sending SUBACK to data_sampler
1677837755: Received SUBSCRIBE from data_sampler
1677837755:     diagstack/response/readdatabyperiodicidentifier (QoS 1)
1677837755: data_sampler 1 diagstack/response/readdatabyperiodicidentifier
1677837755: Sending SUBACK to data_sampler
1677837755: Received SUBSCRIBE from data_sampler
1677837755:     diagstack/response/dynamicallydefinedataidentifier (QoS 1)
1677837755: data_sampler 1 diagstack/response/dynamicallydefinedataidentifier
1677837755: Sending SUBACK to data_sampler
1677837759: New connection from 10.89.0.3 on port 1883.
1677837759: New client connected from 10.89.0.3 as auto-43F930C9-0042-C81C-F2B7-9C1BE1F826EE (p2, c1, k60).
1677837759: No will message specified.
1677837759: Sending CONNACK to auto-43F930C9-0042-C81C-F2B7-9C1BE1F826EE (0, 0)
1677837759: Received PUBLISH from auto-43F930C9-0042-C81C-F2B7-9C1BE1F826EE (d0, q0, r0, m0, 'registration/result', ... (142 bytes))
1677837759: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'registration/result', ... (142 bytes))
1677837759: Received PUBLISH from data_sampler (d0, q1, r1, m32, 'datasampler/notify/setup', ... (31 bytes))
1677837759: Sending PUBACK to data_sampler (m32, rc0)
1677837759: Received DISCONNECT from auto-43F930C9-0042-C81C-F2B7-9C1BE1F826EE
1677837759: Client auto-43F930C9-0042-C81C-F2B7-9C1BE1F826EE disconnected.
1677837769: New connection from 10.89.0.3 on port 1883.
1677837769: New client connected from 10.89.0.3 as auto-D0372F54-B2DD-1216-4CE6-2D44D50BFD64 (p2, c1, k60).
1677837769: No will message specified.
1677837769: Sending CONNACK to auto-D0372F54-B2DD-1216-4CE6-2D44D50BFD64 (0, 0)
1677837769: Received PUBLISH from auto-D0372F54-B2DD-1216-4CE6-2D44D50BFD64 (d0, q0, r0, m0, 'datasampler/config_j1939_cd_hb', ... (1547 bytes))
1677837769: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'datasampler/config_j1939_cd_hb', ... (1547 bytes))
1677837769: Received DISCONNECT from auto-D0372F54-B2DD-1216-4CE6-2D44D50BFD64
1677837769: Client auto-D0372F54-B2DD-1216-4CE6-2D44D50BFD64 disconnected.
1677837770: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/config_status', ... (102 bytes))
1677837770: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/opencommchannel', ... (190 bytes))


1677837802: New connection from 10.89.0.1 on port 1883.
1677837802: New client connected from 10.89.0.1 as MQTTTranslatorClient (p2, c1, k60).
1677837802: No will message specified.
1677837802: Sending CONNACK to MQTTTranslatorClient (0, 0)
1677837802: Received SUBSCRIBE from MQTTTranslatorClient
1677837802:     diagstack/request/opencommchannel (QoS 0)
1677837802: MQTTTranslatorClient 0 diagstack/request/opencommchannel
1677837802: Sending SUBACK to MQTTTranslatorClient
1677837802: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/opencommchannel', ... (195 bytes))
1677837802: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/opencommchannel', ... (195 bytes))
1677837802: Received SUBSCRIBE from MQTTTranslatorClient
1677837802:     diagstack/request/readpgns (QoS 0)
1677837802: MQTTTranslatorClient 0 diagstack/request/readpgns
1677837802: Sending SUBACK to MQTTTranslatorClient
1677837833: Received PINGREQ from data_sampler
1677837833: Sending PINGRESP to data_sampler
1677837862: Received PINGREQ from MQTTTranslatorClient
1677837862: Sending PINGRESP to MQTTTranslatorClient
1677837893: Received PINGREQ from data_sampler
1677837893: Sending PINGRESP to data_sampler
1677837922: Received PINGREQ from MQTTTranslatorClient
1677837922: Sending PINGRESP to MQTTTranslatorClient
1677837954: Received PINGREQ from data_sampler
1677837954: Sending PINGRESP to data_sampler
1677837982: Received PINGREQ from MQTTTranslatorClient
1677837982: Sending PINGRESP to MQTTTranslatorClient
1677838015: Received PINGREQ from data_sampler
1677838015: Sending PINGRESP to data_sampler
1677838042: Received PINGREQ from MQTTTranslatorClient
1677838042: Sending PINGRESP to MQTTTranslatorClient
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/opencommchannel', ... (190 bytes))
1677838070: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/opencommchannel', ... (190 bytes))
1677838070: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/opencommchannel', ... (96 bytes))
1677838070: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/opencommchannel', ... (96 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838070: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838070: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838071: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838071: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838071: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838071: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838071: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838131: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838131: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (182 bytes))
1677838131: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838131: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (850 bytes))
1677838131: Received PINGREQ from MQTTTranslatorClient
1677838131: Sending PINGRESP to MQTTTranslatorClient
1677838131: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838191: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838191: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838191: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838191: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838191: Received PINGREQ from MQTTTranslatorClient
1677838191: Sending PINGRESP to MQTTTranslatorClient
1677838191: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838251: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838251: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838251: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838251: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838251: Received PINGREQ from MQTTTranslatorClient
1677838251: Sending PINGRESP to MQTTTranslatorClient
1677838251: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838311: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838311: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838311: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838311: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838311: Received PINGREQ from MQTTTranslatorClient
1677838311: Sending PINGRESP to MQTTTranslatorClient
1677838311: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
1677838371: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838371: Sending PUBLISH to MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/request/readpgns', ... (183 bytes))
1677838371: Received PUBLISH from MQTTTranslatorClient (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838371: Sending PUBLISH to data_sampler (d0, q0, r0, m0, 'diagstack/response/readpgns', ... (851 bytes))
1677838371: Received PINGREQ from MQTTTranslatorClient
1677838371: Sending PINGRESP to MQTTTranslatorClient
1677838371: Received PUBLISH from data_sampler (d0, q0, r0, m0, 'cloudclient/j1939hb', ... (620 bytes))
