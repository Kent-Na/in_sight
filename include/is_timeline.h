/*
 * "in_sight" is_timeline.h
 * 
 * Copyright 2018-2018 Kent Nakajima<kent@tuna-cat.com>
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

namespace is {

namespace timeline {

class Tag {
    std::string _name;
    double _t;

    public:

    Tag(std::string name, double t):_name(name), _t(t) { }
    std::string name() const {
        return _name;
    }
    double t() const {
        return _t;
    }
};

class Segment {
    std::string _name;
    std::vector<Tag> _tags;
    
    public:
    
    Segment(std::string name):_name(name) { }

    std::string name() const {
        return _name;
    }

    Tag& add(std::string name, double t) {
        _tags.emplace_back(name, t);
        return _tags.back();
    }
    Tag& operator[](std::string name);
    auto begin() -> decltype(_tags.begin()) {
        return _tags.begin();
    }
    auto end() -> decltype(_tags.end()) {
        return _tags.end();
    }
};

class Fragment {
    std::vector<Segment> _segments;
    double _t0;

    public:

    Fragment* t0(double t0) {
        _t0 = t0;
        return this;
    }
    double t0() const {
        return _t0;
    }

    Segment& add(std::string name) {
        _segments.emplace_back(name);
        return _segments.back();
    }
    Segment& operator[](std::string name);
    auto begin() -> decltype(_segments.begin()) {
        return _segments.begin();
    }
    auto end() -> decltype(_segments.end()) {
        return _segments.end();
    }
    auto size() -> size_t const {
        return _segments.size();
    }
};

class Data_interface : public is::Data {
public:
    Data_interface() = default;
    virtual ~Data_interface() = 0;
    virtual Fragment* fragment() = 0;
};

Data_interface::~Data_interface() { }

class Data: public Data_interface {
    Fragment _fragment;
public:
    Data(Fragment fragment) : _fragment(fragment) { }
    Fragment* fragment() {
        return &_fragment;
    }
};

class Data_list : public Data_interface {
    std::vector<Fragment> _fragments;
    std::string _scale_name;
    Core* _c; //weak ref;
public:
    Data_list(Core* c, std::vector<Fragment> fragments, std::string scale_name) : _c(c), _fragments(fragments), _scale_name(scale_name) { }

    Fragment* fragment() {
        if (_fragments.empty()) return nullptr;
        size_t idx = _c->get_scale(_scale_name);
        if (idx > _fragments.size()) idx = _fragments.size()-1;
        return &_fragments[idx];
    }
};


class View:public is::Data_view {
public:
    static constexpr auto segment_h = 30;

    Data_interface *_data;
    float _grid_interval;

    View(Core* c, Fragment fragment):
        _data(new Data(fragment)), _grid_interval(0){ 
        c->add(this);
    }
    View(Core* c, const std::vector<Fragment> &fragments, std::string scale_name):
        _data(new Data_list(c, fragments, scale_name)){ 
        c->add(this);
    }

    size_t min_contents_h(){
        return 64;
    }
    size_t min_contents_w(){
        return 0;
    }
    size_t max_contents_h(){
        return 100;
        if (!_data->fragment()) return 64;
        return segment_h*_data->fragment()->size();
    }
    size_t max_contents_w(){
        return 200;
    }

    float grid_interval() const{
        return _grid_interval;
    }

    View* grid_interval(float value){
        _grid_interval = value;
        return this;
    }

    void update_grid(Size s) const{
        if (_grid_interval < 10){
            color::grid();
            glBegin(GL_LINES);
            glVertex2d(0, 0);
            glVertex2d(0, s.h);
            glEnd();
            return;
        }

        const float grid_start = 0;
        const float grid_end = s.w;
        auto t_from_i = [=](size_t i){return grid_start+_grid_interval*i;};

        color::grid();
        glBegin(GL_LINES);
        for (size_t i = 0; t_from_i(i)<grid_end; i++){
            glVertex2d(t_from_i(i), 0);
            glVertex2d(t_from_i(i), s.h);
        }
        glEnd();
    }

    void update_contents(Core *c){
        Rect f = this->contents_frame();
        GLint original_viewport[4];
        glGetIntegerv(GL_VIEWPORT, original_viewport);
        glPushMatrix();

        {
            Rect f = this->frame();
            glViewport(f.p.x, f.p.y, f.s.w, f.s.h);
            glLoadIdentity();
            glOrtho(0, f.s.w, 0, f.s.h, -100, 100);
        }

        glTranslated(100, 0, 0);

        update_grid(f.s);

        constexpr double t_scale = 3;
        
        auto fragment = _data->fragment();
        if (fragment) {
            auto t0 = fragment->t0();

            Text_texture *tex_gen = shared_text_texture();

            glPushMatrix();
            for (auto& segment : *fragment) {
                auto segment_name = segment.name();
                double t_min = 0;
                double t_max = 0;
                bool init_done = false;
                for (auto& tag : segment) {
                    auto tag_name = tag.name();
                    auto t = tag.t();
                    if (t_min > t || !init_done) t_min = t;
                    if (t_max < t || !init_done) t_max = t;
                    init_done = true;

                    color::value();
                    glBegin(GL_LINES);
                    glVertex2d((t-t0)*t_scale, 7);
                    glVertex2d((t-t0)*t_scale, 14);
                    glEnd();

                    auto x = (t-t0)*t_scale;
                    size_t tex_w = tex_gen->width(14, tag_name.c_str())/4*4+4;
                    Size tex_s = {tex_w, 14};
                    GLuint tex = tex_gen->generate(tex_s, tag_name.c_str());
                    color::text();
                    draw_texture(tex, x, 12, tex_s.w, tex_s.h);
                    glDeleteTextures(1, &tex);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }

                auto x_min = (t_min-t0)*t_scale;
                auto x_max = (t_max-t0)*t_scale;

                size_t tex_w = tex_gen->width(14, segment_name.c_str())/4*4+4;
                Size tex_s = {tex_w, 14};
                GLuint tex = tex_gen->generate(tex_s, segment_name.c_str());
                draw_texture(tex, x_min-tex_s.w, 0, tex_s.w, tex_s.h);
                glDeleteTextures(1, &tex);
                glBindTexture(GL_TEXTURE_2D, 0);

                glBegin(GL_LINES);
                glVertex2d(x_min, 10);
                glVertex2d(x_max, 10);
                glEnd();

                glTranslated(0, 30, 0);
            }
            glPopMatrix();
        }

        glPopMatrix();

        glViewport(
            original_viewport[0],
            original_viewport[1],
            original_viewport[2],
            original_viewport[3]);
    }
};
}
}
