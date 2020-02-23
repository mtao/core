#pragma once
#include <tbb/parallel_reduce.h>
#include <tbb/tbb_stddef.h>
#include <tbb/blocked_range.h>
#include <tbb/tbb_config.h>
#include <tbb/partitioner.h>



namespace mtao::tbb {
    namespace internal {
        // copied from tbb's parallel_reduce but enables passing rvalues
        template<typename Range, typename Value, typename RealBody, typename Reduction>
            class rvalue_lambda_reduce_body {

                //FIXME: decide if my_real_body, my_reduction, and identity_element should be copied or referenced
                //       (might require some performance measurements)

                const Value&     identity_element;
                const RealBody&  my_real_body;
                const Reduction& my_reduction;
                Value            my_value;
                rvalue_lambda_reduce_body& operator= ( const rvalue_lambda_reduce_body& other );
                public:
                rvalue_lambda_reduce_body( const Value& identity, const RealBody& body, const Reduction& reduction )
                    : identity_element(identity)
                      , my_real_body(body)
                      , my_reduction(reduction)
                      , my_value(identity)
                { }
                rvalue_lambda_reduce_body( const rvalue_lambda_reduce_body& other )
                    : identity_element(other.identity_element)
                      , my_real_body(other.my_real_body)
                      , my_reduction(other.my_reduction)
                      , my_value(other.my_value)
                { }
                rvalue_lambda_reduce_body( rvalue_lambda_reduce_body& other, ::tbb::split )
                    : identity_element(other.identity_element)
                      , my_real_body(other.my_real_body)
                      , my_reduction(other.my_reduction)
                      , my_value(other.identity_element)
                { }
                void operator()(Range& range) {
                    my_value = my_real_body(range, std::move(my_value));
                }
                void join( rvalue_lambda_reduce_body&& rhs ) {
                    my_value = my_reduction(std::move(my_value), std::move(rhs.my_value));
                }
                const Value& result() const {
                    return my_value;
                }
            };
    }

    template <typename Range, typename Value, typename RealBody, typename Reduction>
        Value parallel_reduce( const Range& range, const Value& identity, const RealBody& real_body, const Reduction& reduction ) {
            using BodyType =  internal::rvalue_lambda_reduce_body<Range,Value,RealBody,Reduction>;
            BodyType body(identity, real_body, reduction);
            ::tbb::internal::start_reduce<Range,BodyType,const ::tbb::auto_partitioner>
                ::run(range, body, ::tbb::auto_partitioner() );
            return body.result();
        }
}
