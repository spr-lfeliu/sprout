package com.spr.sprout.repository

import com.spr.sprout.entity.DeviceData
import org.springframework.data.repository.CrudRepository
import org.springframework.stereotype.Repository

@Repository
interface DeviceDataRepository extends CrudRepository<DeviceData, Long> {
    List<DeviceData> findByDeviceId(id);
}
