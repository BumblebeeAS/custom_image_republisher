# Custom Image Republisher

An implementation of [image republisher](https://github.com/ros-perception/image_common/tree/humble/image_transport) with default QoS set as `rmw_qos_profile_sensor_data`.

**Note:** QoS overriding for the `republish` node is available on `jazzy` and later releases, but has not been backported to `humble`. If upgrading the ROS version to `jazzy` or later releases, or if QoS overriding is backported on the official ROS `image_common` repository, try that instead.
