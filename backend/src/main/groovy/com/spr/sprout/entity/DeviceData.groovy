package com.spr.sprout.entity

import javax.persistence.Column
import javax.persistence.Entity
import javax.persistence.GeneratedValue
import javax.persistence.GenerationType
import javax.persistence.Id
import javax.persistence.Table

@Entity
@Table(name = "device_data")
class DeviceData {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "id")
    Integer id

    @Column(name = "device_id")
    String deviceId

    @Column(name = "soil_temp")
    Double soilTemp;

    @Column(name = "soil_moisture")
    Double soilMoisture

    @Column(name = "air_temp")
    Double airTemperature

    @Column(name = "air_moisture")
    Double airMoisture

    @Column(name = "light")
    Double light

    Integer getId() {
        return id
    }

    void setId(Integer id) {
        this.id = id
    }

    String getDeviceId() {
        return deviceId
    }

    void setDeviceId(String deviceId) {
        this.deviceId = deviceId
    }

    Double getSoilTemp() {
        return soilTemp
    }

    void setSoilTemp(Double soilTemp) {
        this.soilTemp = soilTemp
    }

    Double getSoilMoisture() {
        return soilMoisture
    }

    void setSoilMoisture(Double soilMoisture) {
        this.soilMoisture = soilMoisture
    }

    Double getAirTemperature() {
        return airTemperature
    }

    void setAirTemperature(Double airTemperature) {
        this.airTemperature = airTemperature
    }

    Double getAirMoisture() {
        return airMoisture
    }

    void setAirMoisture(Double airMoisture) {
        this.airMoisture = airMoisture
    }

    Double getLight() {
        return light
    }

    void setLight(Double light) {
        this.light = light
    }
}

