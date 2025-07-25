// Copyright (c) 2009, Willow Garage, Inc.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Willow Garage nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <memory>
#include <string>
#include <utility>

#include "pluginlib/class_loader.hpp"

#include "rclcpp/rclcpp.hpp"

#include "image_transport/image_transport.hpp"
#include "image_transport/publisher_plugin.hpp"

namespace custom_image_republisher
{
class Republisher : public rclcpp::Node
{
public:
  explicit Republisher(const rclcpp::NodeOptions & options)
  : Node("image_republisher", options)
  {
    std::string in_transport = "raw";
    this->declare_parameter<std::string>("in_transport", in_transport);
    if (!this->get_parameter(
        "in_transport", in_transport))
    {
      RCLCPP_WARN_STREAM(
        this->get_logger(),
        "The 'in_transport' parameter was not defined." << in_transport);
    } else {
      RCLCPP_INFO_STREAM(
        this->get_logger(),
        "The 'in_transport' parameter is set to: " << in_transport);
    }

    std::string out_transport = "";
    this->declare_parameter<std::string>("out_transport", out_transport);
    if (!this->get_parameter(
        "out_transport", out_transport))
    {
      RCLCPP_WARN_STREAM(
        this->get_logger(),
        "The parameter 'out_transport' was not defined." << out_transport);
    } else {
      RCLCPP_INFO_STREAM(
        this->get_logger(),
        "The 'out_transport' parameter is set to: " << out_transport);
    }

    std::string in_topic = rclcpp::expand_topic_or_service_name("in", get_name(), get_namespace());
    std::string out_topic = rclcpp::expand_topic_or_service_name("out", get_name(), get_namespace());

    if (out_transport.empty()) {
      // Use all available transports for output
      pub_ = image_transport::create_publisher(this, out_topic, rmw_qos_profile_sensor_data);

      // Use Publisher::publish as the subscriber callback
      typedef void (image_transport::Publisher::* PublishMemFn)(
      const sensor_msgs::msg::Image::ConstSharedPtr &) const;
      PublishMemFn pub_mem_fn = &image_transport::Publisher::publish;

      sub_ = image_transport::create_subscription(
        this, in_topic,
        std::bind(pub_mem_fn, &pub_, std::placeholders::_1),
        in_transport,
        rmw_qos_profile_sensor_data
      );
    } else {
      // Use one specific transport for output
      // Load transport plugin
      typedef image_transport::PublisherPlugin Plugin;
      loader = std::make_shared<pluginlib::ClassLoader<Plugin>>(
        "image_transport",
        "image_transport::PublisherPlugin");
      std::string lookup_name = Plugin::getLookupName(out_transport);

      this->instance = loader->createUniqueInstance(lookup_name);
      this->instance->advertise(this, out_topic, rmw_qos_profile_sensor_data);

      // Use PublisherPlugin::publish as the subscriber callback
      typedef void (Plugin::* PublishMemFn)(const sensor_msgs::msg::Image::ConstSharedPtr &) const;
      PublishMemFn pub_mem_fn = &Plugin::publishPtr;
      sub_ = image_transport::create_subscription(
        this, in_topic,
        std::bind(pub_mem_fn, this->instance.get(), std::placeholders::_1),
        in_transport,
        rmw_qos_profile_sensor_data
      );
    }
  }

private:
  image_transport::Subscriber sub_;
  image_transport::Publisher pub_;
  pluginlib::UniquePtr<image_transport::PublisherPlugin> instance;
  std::shared_ptr<pluginlib::ClassLoader<image_transport::PublisherPlugin>> loader;
};

}  // namespace custom_image_republisher

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(custom_image_republisher::Republisher)
