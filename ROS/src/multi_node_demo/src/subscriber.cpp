#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <memory>

class CombinedSubscriber : public rclcpp::Node{
public:
    CombinedSubscriber() : Node("combined_subscriber"){
        student_subscription_ = this->create_subscription<std_msgs::msg::String>(
            "student_info", 10,
            std::bind(&CombinedSubscriber::student_callback, this, std::placeholders::_1));
        
        thoughts_subscription_ = this->create_subscription<std_msgs::msg::String>(
            "ros_thoughts", 10,
            std::bind(&CombinedSubscriber::thoughts_callback, this, std::placeholders::_1));
        
        RCLCPP_INFO(this->get_logger(), "================================");
        RCLCPP_INFO(this->get_logger(), "等待接收学生信息和ROS想法...");
        RCLCPP_INFO(this->get_logger(), "订阅话题:");
        RCLCPP_INFO(this->get_logger(), "  • student_info");
        RCLCPP_INFO(this->get_logger(), "  • ros_thoughts");
        RCLCPP_INFO(this->get_logger(), "================================");
    }

private:
    void student_callback(const std_msgs::msg::String::SharedPtr msg) const{
        RCLCPP_INFO(this->get_logger(), "收到学生信息:");
        RCLCPP_INFO(this->get_logger(), "    %s", msg->data.c_str());
        RCLCPP_INFO(this->get_logger(), "--------------------------------");
    }
    
    void thoughts_callback(const std_msgs::msg::String::SharedPtr msg) const{
        RCLCPP_INFO(this->get_logger(), "收到ROS想法:");
        RCLCPP_INFO(this->get_logger(), "    %s", msg->data.c_str());
        RCLCPP_INFO(this->get_logger(), "--------------------------------");
    }
    
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr student_subscription_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr thoughts_subscription_;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CombinedSubscriber>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}