/**
 * @file       test_proto.cpp
 * @date       2023/9/13
 * @since      2023-09-13
 * @author     czh
 */

#include "test.pb.h"
#include <google/protobuf/message.h>
#include <spdlog/spdlog.h>
#include <tuple>
#include <type_traits>


void test_reflect()
{
    std::string message_name    = "tutorial.Person";
    std::string field_name      = "name";
    const auto* descriptor_pool = google::protobuf::DescriptorPool::generated_pool();
    const auto* descriptor      = descriptor_pool->FindMessageTypeByName(message_name);

    if (descriptor) {
        auto* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            auto instance = prototype->New();
            spdlog::info(
                "Message descriptor full_name:{},descriptor name:{},Message instance typename:{}",
                descriptor->full_name(),
                descriptor->name(),
                instance->GetTypeName());
            const auto* des       = instance->GetDescriptor();
            const auto* ref       = instance->GetReflection();
            auto*       field_des = des->FindFieldByName(field_name);
            ref->SetString(const_cast<google::protobuf::Message*>(instance), field_des, "zhangsan");
            // 此处可以使用tuple 存储函数类型来进行转换
            auto* real_instance = dynamic_cast<tutorial::Person*>(instance);
            spdlog::info("person name:{}", real_instance->name());
        }
    }
}

void test_tuple_cast()
{
    std::string message_name    = "tutorial.Person";
    std::string field_name      = "name";
    const auto* descriptor_pool = google::protobuf::DescriptorPool::generated_pool();
    const auto* descriptor      = descriptor_pool->FindMessageTypeByName(message_name);

    if (descriptor) {
        auto* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            auto instance = prototype->New();
            spdlog::info("Message type name:{},Message instance:{}",
                         descriptor->full_name(),
                         instance->GetTypeName());
            const auto* des       = instance->GetDescriptor();
            const auto* ref       = instance->GetReflection();
            auto*       field_des = des->FindFieldByName(field_name);
            ref->SetString(const_cast<google::protobuf::Message*>(instance), field_des, "zhangsan");
            auto* real_instance = dynamic_cast<tutorial::Person*>(instance);
            spdlog::info("person name:{}", real_instance->name());
        }
    }
}
template<typename T>
struct traits
{
    using bare_type = std::tuple<std::remove_const_t<std::remove_reference_t<T>>>;
};


void test_pb_value()
{
    tutorial::Person person;
    if (person.has_name()) {
        spdlog::info("person has name");
    }
    else {
        spdlog::info("person has defualt name");
    }

    person.set_name("lisi");
    if (person.has_name()) {
        spdlog::info("person has name:{}", person.name());
    }
    else {
        spdlog::info("person has default name");
    }
    tutorial::Test test;
    if (test.has_price()) {
        spdlog::info("Test has price:{}", test.has_price());
    }
}


int main()
{
    //    test_reflect();
    test_pb_value();
    return 0;
}
