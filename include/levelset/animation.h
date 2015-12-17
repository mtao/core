#ifndef ANIMATION_H
#define ANIMATION_H
#include "transformer.h"


template <int _D>
class LevelsetTransformer: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
        using LTPtr = std::shared_ptr<LevelsetTransformer<_D>>;
        const LTPtr& get_section(Scalar t) {
            if(m_sections.size() == 0) {
                return LTPtr();
            }
            LTPtr ret = m_sections.front();
            for(auto&& s: m_sections) {
                if(t > s->start()) {
                    ret = s;
                }
            }
            return ret;
        }
    private:
    std::vector<LTPtr> m_sections;
};


#endif//ANIMATION_H
