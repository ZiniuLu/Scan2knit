/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include <sstream>
#include <string>
#include "Display.h"
#include <vector>
#include <algorithm>

BEGIN_PROJECT_NAMESPACE

// IglColor::
RVec_3d IglColor::rgb(uchar r, uchar g, uchar b)
{
    return RVec_3d(r / 256, g / 256, b / 256);
}
RVec_3d IglColor::black()
{
    return RVec_3d(0, 0, 0);
}
RVec_3d IglColor::white()
{
    return RVec_3d(1, 1, 1);
}
RVec_3d IglColor::red()
{
    return RVec_3d(1, 0, 0);
}
RVec_3d IglColor::lime()
{
    return RVec_3d(0, 1, 0);
}
RVec_3d IglColor::blue()
{
    return RVec_3d(0, 0, 1);
}
RVec_3d IglColor::yellow()
{
    return RVec_3d(1, 1, 0);
}
RVec_3d IglColor::cyan()
{
    return RVec_3d(0, 1, 1);
}
RVec_3d IglColor::magenta()
{
    return RVec_3d(1, 0, 1);
}
RVec_3d IglColor::silver()
{
    return RVec_3d(0.75, 0.75, 0.75);
}
RVec_3d IglColor::gray()
{
    return RVec_3d(0.5, 0.5, 0.5);
}
RVec_3d IglColor::maroon()
{
    return RVec_3d(0.5, 0, 0);
}
RVec_3d IglColor::olive()
{
    return RVec_3d(0.5, 0.5, 0);
}
RVec_3d IglColor::green()
{
    return RVec_3d(0, 0.5, 0);
}
RVec_3d IglColor::purple()
{
    return RVec_3d(0.5, 0, 0.5);
}
RVec_3d IglColor::teal()
{
    return RVec_3d(0, 0.5, 0.5);
}
RVec_3d IglColor::navy()
{
    return RVec_3d(0, 0, 0.5);
}




// BoundingBox
void BoundingBox::set(MAT_3d& V)
{
    size_t numV = V.rows();
    if (numV <= 0) { return; }

    this->x_min = V(0, 0);
    this->x_max = V(0, 0);
    this->y_min = V(0, 1);
    this->y_max = V(0, 1);
    this->z_min = V(0, 2);
    this->z_max = V(0, 2);

    for (int i = 1; i < numV; ++i)
    {
        this->x_min = (V(i, 0) < x_min) ? V(i, 0) : this->x_min;
        this->x_max = (V(i, 0) > x_max) ? V(i, 0) : this->x_max;
        this->y_min = (V(i, 1) < y_min) ? V(i, 1) : this->y_min;
        this->y_max = (V(i, 1) > y_max) ? V(i, 1) : this->y_max;
        this->z_min = (V(i, 2) < z_min) ? V(i, 2) : this->z_min;
        this->z_max = (V(i, 2) > z_max) ? V(i, 2) : this->z_max;
    }
}




// Display::
Display::Display()                                { }
Display::Display(std::string path)                { this->display(path); }
Display::Display(MAT_3d& V, MAT_2i& E, MAT_3i& F) { this->display(V, E, F); }
Display::Display(MAT_3d& V, MAT_3i& F)            { this->display(V, F); }
Display::Display(MAT_3d& V, MAT_2i& E)            { this->display(V, E); }
Display::Display(Triangle_mesh& tmesh)            { this->display_tmesh(tmesh); }
Display::Display(Skeleton& skeleton)              { this->display_skeleton(skeleton); }
Display::Display(Skel& skel)                      { this->display_skel(skel); }
Display::Display(SkelGraph& skel_graph)           { this->display_skel_graph(skel_graph); }
Display::Display(Stitches& stitches)              { this->display_stitches(stitches); }


// public
void Display::display(std::string path)
{
    size_t pos_max = path.length();
    if (path.find(".off") < pos_max)
    {
        this->display_off(path);
    }
    else if (path.find(".obj") < pos_max)
    {
        this->display_obj(path);
    }
    else if (path.find(".stl") < pos_max)
    {
        this->display_stl(path);
    }
}
void Display::display(MAT_3d& V, MAT_2i& E, MAT_3i& F)
{
    int numV = V.rows();
    int numE = E.rows();
    int numF = F.rows();

    // set para for current viewer
    set_parameter();

    // lauch current viewer
    this->viewer.data().set_mesh(V, F);

    // add edges
    for (unsigned i = 0; i < numE; ++i)
    {
        int index1 = E(i, 0) - 1;   // source vertex number
        int index2 = E(i, 1) - 1;   // target vertex number

        auto& it_source = V.row(index1);
        auto& it_target = V.row(index2);
        auto color_eg = IglColor::red();

        this->viewer.data().add_edges(it_source, it_target, color_eg);
        this->viewer.launch();
    }
}
void Display::display(MAT_3d& V, MAT_3i& F)
{
    int numV = V.rows();
    int numF = F.rows();

    // set para for current viewer
    set_parameter();

    // lauch current viewer
    this->viewer.data().set_mesh(V, F);
    this->viewer.launch();
}
void Display::display(MAT_3d& V, MAT_2i& E)
{
    int numV = V.rows();
    int numE = E.rows();

    // set para for current viewer
    set_parameter();

    // add points
    auto color_v = IglColor::black();
    this->viewer.data().add_points(V, color_v);

    // add edges
    for (unsigned i = 0; i < numE; ++i)
    {
        int index1 = E(i, 0);   // source vertex number
        int index2 = E(i, 1);   // target vertex number

        auto& it_source = V.row(index1);
        auto& it_target = V.row(index2);
        auto color_eg = IglColor::green();

        this->viewer.data().add_edges(it_source, it_target, color_eg);
    }
    this->viewer.launch();
}
void Display::display(MAT_3d& V, MAT_2i& E, MAT_3i& F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F)
{
    
}
void Display::display(MAT_3d& V, MAT_3i& F, RVec_3d color_V, RVec_3d color_F)
{
    //this->viewer.data().set_colors()
}
void Display::display(MAT_3d& V, MAT_2i& E, RVec_3d color_V, RVec_3d color_E)
{
    int numV = V.rows();
    int numE = E.rows();

    // set para for current viewer
    set_parameter();

    // add points
    this->viewer.data().add_points(V, color_V);

    // add edges
    for (unsigned i = 0; i < numE; ++i)
    {
        int index1 = E(i, 0);   // source vertex number
        int index2 = E(i, 1);   // target vertex number

        auto& it_source = V.row(index1);
        auto& it_target = V.row(index2);

        this->viewer.data().add_edges(it_source, it_target, color_E);
    }

    // Attach a menu plugin
    this->set_menu(); 

    this->viewer.launch();
}


void Display::display(Triangle_mesh& tmesh)
{
    this->display_tmesh(tmesh);
}
void Display::display(Skeleton& skeleton)
{
    this->display_skeleton(skeleton);
}
void Display::display(Skel& skel)
{
    this->display_skel(skel);
}
void Display::display(SkelGraph& skel_graph)
{
    this->display_skel_graph(skel_graph);
}

void Display::display(Stitches& stitches)
{
    this->display_stitches(stitches);
}
void Display::display(Stitches& stitches, size_t pos)
{
    display_stitches(stitches, pos);
}
void Display::display(Stitches& stitches, size_t pos, size_t len)
{
    display_stitches(stitches, pos, len);
}


// private
void Display::display_obj(std::string path)
{
    std::ifstream input;
    input.open(path, std::ios::in);

    if (!input.is_open())
    {
        std::cout << "Cannot open file \"" << path << "\"!";
        return;
    }

    std::string line = "";
    std::vector<std::string> buf;
    std::vector<std::vector<double>> v;
    std::vector<std::vector<int>> e;
    std::vector<std::vector<int>> f;
    
    while (std::getline(input, line))
    {
        buf.clear();

        // divide elements in each line
        std::stringstream inputL(line);
        std::string str;
        while (inputL >> str)
        {
            buf.push_back(str);
        }

        if (buf.size() == 0) { continue; }
        
        auto it_buf_begin = buf.begin();
        std::string line_type = *it_buf_begin;

        if (line_type == "v")
        {
            std::vector<double> v_;
            v_.resize(3);
            v_[0] = stod(*(it_buf_begin + 1));
            v_[1] = stod(*(it_buf_begin + 2));
            v_[2] = stod(*(it_buf_begin + 3));
            v.push_back(v_);
        }
        else if (line_type == "l")
        {
            std::vector<int> e_;
            e_.resize(2);
            e_[0] = stoi(*(it_buf_begin + 1)) - 1;
            e_[1] = stoi(*(it_buf_begin + 2)) - 1;
            e.push_back(e_);
        }
        else if (line_type == "f")
        {
            std::vector<int> f_;
            f_.resize(3);
            f_[0] = stoi(*(it_buf_begin + 1)) - 1;
            f_[1] = stoi(*(it_buf_begin + 2)) - 1;
            f_[2] = stoi(*(it_buf_begin + 3)) - 1;
            f.push_back(f_);
        }
    }

    input.close();

    int numV = v.size();
    int numE = e.size();
    int numF = f.size();

    if (numV > 0)
    {
        this->V.resize(numV, 3);
        for (int i = 0; i < numV; i++)
        {
            //this->V.row(i) = RVec_3d(v[i][0], v[i][1], v[i][2]);
            (this->V)(i, 0) = v[i][0];
            (this->V)(i, 1) = v[i][1];
            (this->V)(i, 2) = v[i][2];
        }
    }
    if (numE > 0)
    {
        this->E.resize(numE, 2);
        for (int i = 0; i < numE; i++)
        {
            //this->E.row(i) = RVec_2i(e[i][0], e[i][1]);
            (this->E)(i, 0) = e[i][0];
            (this->E)(i, 1) = e[i][1];
        }
    }
    if (numF > 0)
    {
        this->F.resize(numF, 3);
        for (int i = 0; i < numF; i++)
        {
            //this->F.row(i) = RVec_3i(f[i][0], f[i][1], f[i][2]);
            (this->F)(i, 0) = f[i][0];
            (this->F)(i, 1) = f[i][1];
            (this->F)(i, 2) = f[i][2];
        }
    }

    if (numE == 0 && numF > 0)      
    { 
        display(this->V, this->F); 
    }
    else if (numE > 0 && numF == 0)
    { 
        display(this->V, this->E); 
    }
    else if (numE > 0 && numF > 0)  
    { 
        display(this->V, this->E, this->F); 
    }
}

void Display::display_off(std::string path)
{
    igl::readOFF(path, this->V, this->F);
    this->viewer.data().set_mesh(this->V, this->F);
    this->viewer.launch();
}
void Display::display_stl(std::string path)
{
    std::ifstream input(path);
    MAT_3d N;
    igl::readSTL(input, this->V, this->F, N);

    this->viewer.data().set_mesh(this->V, this->F);
    this->viewer.launch();

}

void Display::display_tmesh(Triangle_mesh& tmesh)
{
    int numV = tmesh.number_of_vertices();
    int numF = tmesh.number_of_faces();

    this->V.resize(numV, 3);
    this->F.resize(numF, 3);


    // add vertices
    size_t v_i = 0;
    for (const auto& v : tmesh.vertices())
    {
        const Point& p = tmesh.point(v);
        //this->V.row(v_i) = RVec_3d(p.x(), p.y(), p.z());
        (this->V)(v_i, 0) = p.x();
        (this->V)(v_i, 1) = p.y();
        (this->V)(v_i, 2) = p.z();
        ++v_i;
    }

    // add facets
    size_t f_i = 0;
    for (const face_descriptor& fd : tmesh.faces())
    {
        size_t axis = 0;
        for (const vertex_descriptor& vd : CGAL::vertices_around_face(tmesh.halfedge(fd), tmesh))
        {
            (this->F)(f_i, axis) = vd;
            ++axis;
        }
        //std::cout << "face " << f_i << ": " 
        //          << (this->F)(f_i, 0) << "\t"
        //          << (this->F)(f_i, 1) << "\t"
        //          << (this->F)(f_i, 2) << "\t"
        //          << std::endl;
        ++f_i;
    }

    this->viewer.data().set_mesh(this->V, this->F);
    this->viewer.launch();
}

void Display::display_skeleton(Skeleton& skeleton)
{
    //Skel skel(skeleton);
    //this->display_skel(skel);
}
void Display::display_skel(Skel& skel)
{
    //SkelGraph skel_graph(skel);
    //this->display_skel_graph(skel_graph);
}
void Display::display_skel_graph(SkelGraph& skel_graph)
{
    const std::vector<SkelNode>& skel_nodes = skel_graph.get_skel_nodes();
    const std::vector<SkelEdge>& skel_edges = skel_graph.get_skel_edges();

    size_t numV = skel_nodes.size();
    size_t numE = skel_edges.size();

    this->V.resize(numV, 3);
    this->E.resize(numE, 2);

    size_t v_i = 0;
    for (SkelNode nd : skel_nodes)
    {
        const Point& p = nd.point();

        (this->V)(v_i, 0) = p.x();
        (this->V)(v_i, 1) = p.y();
        (this->V)(v_i, 2) = p.z();

        ++v_i;
    }
    size_t e_i = 0;
    for (SkelEdge eg : skel_edges)
    {
        (this->E)(e_i, 0) = eg.source() - 1;
        (this->E)(e_i, 1) = eg.target() - 1;

        ++e_i;
    }

    RVec_3d color_v = IglColor::black();
    RVec_3d color_e = IglColor::yellow();
    this->show_menu = true;

    this->display(V, E, color_v, color_e);
}

void Display::display_stitches(Stitches& stitches)
{
    display_stitches(stitches, 0, 0);
}
void Display::display_stitches(Stitches& stitches, size_t pos)
{
    display_stitches(stitches, pos, 0);
}
void Display::display_stitches(Stitches& stitches, size_t pos, size_t len)
{
    auto& stitches_ = stitches.get();
    uint32_t numV = stitches_.size();
    uint32_t numE;

    IGL_Viewer viewer;
    //viewer.data().line_width = 4;
    //viewer.data().point_size = 6;

    // add vertices
    MAT_3d V;
    V.resize(numV, 3);
    for (auto& st : stitches_)
    {
        auto& idx = st.index;
        auto& v = st.vertex;
        auto& color_v = st.color;

        V(idx, 0) = v(0);
        V(idx, 1) = v(1);
        V(idx, 2) = v(2);

        viewer.data().add_points(v, color_v);
        //std::cout << "V" << idx << ": " << v << std::endl;
    }
    //viewer.data().add_points(V, IglColor::yellow()); // yellow

    // add edges
    auto it_stitches_begin = stitches_.begin();
    auto it_stitches_end = stitches_.end();

    size_t len_max = it_stitches_end - it_stitches_begin;
    auto it_line_begin = (pos > 0 && pos < len_max) ? it_stitches_begin + pos : it_stitches_begin;
    auto it_line_end = (len > 0 && (pos + len) < len_max) ? it_line_begin + len : it_stitches_end;

    auto it_stitches = it_line_begin;
    auto it_last_stitch = it_line_begin;


    for (; it_stitches != it_line_end; ++it_stitches)
    {
        auto& idx_last = it_last_stitch->index;
        auto& idx = it_stitches->index;

        if (idx == 0) { continue; }
        else
        {
            auto& it_source = V.row(idx_last);
            auto& it_target = V.row(idx);

            RVec_3d color_eg;
            if (it_last_stitch->direction == 'c' && it_stitches->direction == 'c')
            {
                color_eg = (idx_last == 0) ? IglColor::red() : IglColor::white();
            }
            else if (it_last_stitch->direction == 'a' && it_stitches->direction == 'a')
            {
                color_eg = (idx_last == 0) ? IglColor::red() : IglColor::blue();
            }

            viewer.data().add_edges(it_source, it_target, color_eg);
            //std::cout << "E " << idx_last << " -> " << idx << std::endl;
        }

        switch (it_stitches->type)
        {
        case 'e':
        case 't':
        case 'm':
        case 'k':
        case 'i':
        {
            uint32_t in_0 = it_stitches->in_0;
            auto& it_source = V.row(in_0);
            auto& it_target = V.row(idx);
            auto& color_eg = IglColor::green();

            viewer.data().add_edges(it_source, it_target, color_eg);
            //std::cout << "E " << in_0 << " -> " << idx << std::endl;

            break;
        }
        case 'd':
        {
            uint32_t in_0 = it_stitches->in_0;
            uint32_t in_1 = it_stitches->in_1;

            auto& it_source_0 = V.row(in_0);
            auto& it_source_1 = V.row(in_1);
            auto& it_target = V.row(idx);
            auto& color_eg = IglColor::green();

            viewer.data().add_edges(it_source_0, it_target, color_eg);
            viewer.data().add_edges(it_source_1, it_target, color_eg);
            //std::cout << "E " << in_0 << " -> " << idx << std::endl;
            //std::cout << "E " << in_1 << " -> " << idx << std::endl;

            break;
        }
        }

        it_last_stitch = it_stitches;
    }

    viewer.launch();
}

void Display::set_parameter()
{
    if (V.rows() <= 0) { return; }
    BoundingBox box;
    box.set(this->V);
    this->size = RVec_3d(box.x_max - box.x_min, box.y_max - box.y_min, box.z_max - box.z_min);
    double volume = this->size(0) * this->size(1) * this->size(2);

    extern Settings settings;
    this->viewer.data().point_size = settings.Viewer.point_size;
    this->viewer.data().line_width = settings.Viewer.line_width;
    this->viewer.core().camera_zoom = settings.Viewer.camera_zoom;

    std::cout << "\nBounding box volumen = " << volume << "\n"
              << "\t x_min = " << box.x_min << "\t x_max = " << box.x_max << "\t x_length = " << size(0) << "\n"
              << "\t y_min = " << box.y_min << "\t y_max = " << box.y_max << "\t y_length = " << size(1) << "\n"
              << "\t z_min = " << box.z_min << "\t z_max = " << box.z_max << "\t z_length = " << size(2) << "\n"
              << std::endl;
}

void Display::set_menu()
{
    if (!show_menu) { return; }

    this->viewer.plugins.push_back(&(this->menu));

    double doubleVariable = 0.1f; // Shared between two menus

  // Add content to the default menu window
    this->menu.callback_draw_viewer_menu = [&]()
    {
        // Draw parent menu content
        this->menu.draw_viewer_menu();

        // Add new group
        if (ImGui::CollapsingHeader("New Group", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Expose variable directly ...
            ImGui::InputDouble("double", &doubleVariable, 0, 0, "%.4f");

            // ... or using a custom callback
            static bool boolVariable = true;
            if (ImGui::Checkbox("bool", &boolVariable))
            {
                // do something
                std::cout << "boolVariable: " << std::boolalpha << boolVariable << std::endl;
            }

            // Expose an enumeration type
            enum Orientation { Up = 0, Down, Left, Right };
            static Orientation dir = Up;
            ImGui::Combo("Direction", (int*)(&dir), "Up\0Down\0Left\0Right\0\0");

            // We can also use a std::vector<std::string> defined dynamically
            static int num_choices = 3;
            static std::vector<std::string> choices;
            static int idx_choice = 0;
            if (ImGui::InputInt("Num letters", &num_choices))
            {
                num_choices = std::max(1, std::min(26, num_choices));
            }
            if (num_choices != (int)choices.size())
            {
                choices.resize(num_choices);
                for (int i = 0; i < num_choices; ++i)
                    choices[i] = std::string(1, 'A' + i);
                if (idx_choice >= num_choices)
                    idx_choice = num_choices - 1;
            }
            ImGui::Combo("Letter", &idx_choice, choices);

            // Add a button
            if (ImGui::Button("Print Hello", ImVec2(-1, 0)))
            {
                std::cout << "Hello\n";
            }
        }
    };

    // Draw additional windows
    this->menu.callback_draw_custom_window = [&]()
    {
        // Define next window position + size
        ImGui::SetNextWindowPos(ImVec2(180.f * this->menu.menu_scaling(), 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 160), ImGuiCond_FirstUseEver);
        ImGui::Begin(
            "New Window", nullptr,
            ImGuiWindowFlags_NoSavedSettings
        );

        // Expose the same variable directly ...
        ImGui::PushItemWidth(-80);
        ImGui::DragScalar("double", ImGuiDataType_Double, &doubleVariable, 0.1, 0, 0, "%.4f");
        ImGui::PopItemWidth();

        static std::string str = "bunny";
        ImGui::InputText("Name", str);

        ImGui::End();
    };
}

END_PROJECT_NAMESPACE

