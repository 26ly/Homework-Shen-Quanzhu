#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>

using namespace std::chrono_literals;

class StudentPublisher : public rclcpp::Node{
public:
    StudentPublisher() : Node("student_publisher"){
        
        publisher_ = this->create_publisher<std_msgs::msg::String>("student_info", 10);
        
        timer_ = this->create_wall_timer(1s, std::bind(&StudentPublisher::timer_callback, this));
        
        RCLCPP_INFO(this->get_logger(), "学生信息发布者启动");
    }

private:
    void timer_callback(){
        auto message = std_msgs::msg::String();
        message.data = "姓名: 沈泉竹, 学院: 光学与电子信息学院";
     
        publisher_->publish(message);
        RCLCPP_INFO(this->get_logger(), "发布: %s", message.data.c_str());
    }
    
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;

};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<StudentPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}