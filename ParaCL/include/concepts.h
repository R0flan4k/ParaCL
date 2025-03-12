#pragma once

#include <memory>

template <typename Ptr> concept smart_pointer = requires(Ptr p)
{
    typename std::remove_cvref_t<Ptr>::element_type;
}
&&std::disjunction_v<
    std::is_same<
        std::remove_cvref_t<Ptr>,
        std::shared_ptr<typename std::remove_cvref_t<Ptr>::element_type>>,
    std::is_same<
        std::remove_cvref_t<Ptr>,
        std::weak_ptr<typename std::remove_cvref_t<Ptr>::element_type>>,
    std::is_same<
        std::remove_cvref_t<Ptr>,
        std::unique_ptr<typename std::remove_cvref_t<Ptr>::element_type>>>;