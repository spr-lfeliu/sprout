package com.spr.sprout.controller

import com.spr.sprout.azure.AzureIoTHubService
import com.spr.sprout.entity.DeviceData
import com.spr.sprout.model.Data
import com.spr.sprout.repository.DeviceDataRepository
import org.slf4j.Logger
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.transaction.annotation.Transactional
import org.springframework.web.bind.annotation.*

@RestController
class DeviceDataController {

    private static final Logger log = LoggerFactory.getLogger(this.getClass());

    @Autowired
    DeviceDataRepository deviceDataRepository

    @RequestMapping(value = "/data", method = RequestMethod.POST, consumes = "application/json")
    @Transactional
    void data(@RequestBody(required = true) Data data) {
        DeviceData deviceData = DeviceData.newInstance()
        deviceData.deviceId = data.deviceId
        deviceData.airMoisture = data.airMoisture
        deviceData.airTemperature = data.airTemperature
        deviceData.soilMoisture = data.soilMoisture
        deviceData.soilTemp = data.soilTemperature
        deviceData.light = data.light
        deviceDataRepository.save(deviceData)
        AzureIoTHubService.sendMessageToIoTHub(data)
    }

    @ExceptionHandler(Exception.class)
    void handleException(Exception e) {
        log.error(e.getMessage())
    }
}
