package com.spr.sprout

import org.springframework.boot.SpringApplication
import org.springframework.boot.autoconfigure.EnableAutoConfiguration
import org.springframework.boot.autoconfigure.SpringBootApplication
import org.springframework.boot.context.properties.EnableConfigurationProperties
import org.springframework.context.annotation.ComponentScan
import org.springframework.context.annotation.Configuration

@SpringBootApplication
@EnableAutoConfiguration
@EnableConfigurationProperties
@Configuration
@ComponentScan
class Application {
    static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
