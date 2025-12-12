#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>
#include <vector>

using namespace std::chrono_literals;

class RosThoughtsPublisher : public rclcpp::Node{
public:
    RosThoughtsPublisher() : Node("ros_thoughts_publisher"){
        publisher_ = this->create_publisher<std_msgs::msg::String>("ros_thoughts", 10);
        
        timer_ = this->create_wall_timer(2s, std::bind(&RosThoughtsPublisher::timer_callback, this));
        
        thoughts_ ={
            "ROS2好难啊啊啊啊啊啊啊啊啊啊",
            "配置各种依赖和包真是折磨",
            "ROS2的写法根本看不懂",
            "各种命令行完全记不住",
            "构建流程是什么,到底怎么运行",
            "为什么每次都要source一遍好烦",
            "include path又出问题了",
            "PPT的内容看得我头疼",
            "好不容易啃明白了自己一写还是抓瞎",
            "没有DeepSeek我就是废物"
        };
        
        RCLCPP_INFO(this->get_logger(), "ROS想法发布者启动");
    }

private:
    void timer_callback(){
        auto message = std_msgs::msg::String();
        
        message.data = "想法: " + thoughts_[thought_index_];
        thought_index_ = (thought_index_ + 1) % thoughts_.size();
        
        publisher_->publish(message);
        RCLCPP_INFO(this->get_logger(), "发布: %s", message.data.c_str());
    }
    
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    std::vector<std::string> thoughts_;
    size_t thought_index_ = 0;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RosThoughtsPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}