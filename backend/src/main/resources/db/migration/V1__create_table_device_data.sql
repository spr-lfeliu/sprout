create table device_data (
  id int not null AUTO_INCREMENT,
  device_id varchar(256) not null,
  soil_temp double not null,
  soil_moisture double not null,
  air_temp double not null,
  air_moisture double not null,
  light double not null,
  primary key (id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
