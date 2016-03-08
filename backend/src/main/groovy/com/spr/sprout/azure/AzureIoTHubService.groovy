package com.spr.sprout.azure

import groovy.json.*
import com.microsoft.azure.iothub.*
import com.spr.sprout.model.Data;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

class AzureIoTHubService {

    private static final Logger log = LoggerFactory.getLogger(this.getClass());

    public static final CONNECTION_STRING = "HostName=SPRoutHub.azure-devices.net;DeviceId=SPRout00001;SharedAccessKey=C8BUEHrl9HyoU0npWcF7kXJZoFklH/uAzSP3LbchKmY="

    protected static class EventCallback implements IotHubEventCallback {

        public void execute(IotHubStatusCode status, Object context) {
            log.info("IoT Hub responded to message with status " + status.name())
        }
    }

    public static void sendMessageToIoTHub(Data data) throws IOException, URISyntaxException {
        log.info("Sending message to Azure IoT Hub...")
        Thread.start {
            DeviceClient client = new DeviceClient(AzureIoTHubService.CONNECTION_STRING, IotHubClientProtocol.HTTPS)
            log.info("Successfully created an IoT Hub client.")

            client.open()
            log.info("Opened connection to IoT Hub.")

            log.info("Sending the following event message:")
            String msgStr = new JsonBuilder(data).toPrettyString()
            Message msg = new Message(msgStr)
            log.info(msgStr)

            EventCallback callback = new EventCallback()
            client.sendEventAsync(msg, callback, data.deviceId)
            client.close()
            log.info("Thread finished execution")
        }
    }
}
