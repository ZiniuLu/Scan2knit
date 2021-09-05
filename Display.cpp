/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "Display.h"
#include "Settings.h"


BEGIN_PROJECT_NAMESPACE

//GuiDisplay::
GuiDisplay::GuiDisplay(IGL_Viewer* p_viewer, IGL_Gui* p_gui, std::vector<IglGeometry>* p_geoms)
{
    this->Reset();

    this->viewer = p_viewer;
    this->gui = p_gui;
    this->geoms = p_geoms;
}

void GuiDisplay::Draw(const char* title, bool* p_open)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 260.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200.0f, 500.0f), ImGuiCond_FirstUseEver);
    bool _viewer_menu_visible = true;

    ImGui::Begin(title, &_viewer_menu_visible);

    if (ImGui::CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Select rotation type
        int rotation_type = static_cast<int>(this->viewer->core().rotation_type);
        static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
        static bool orthographic = true;
        ImGui::PushItemWidth(100 * this->gui->menu_scaling());
        if (ImGui::Combo("Camera Type", &rotation_type, "Trackball\0Two Axes\0002D Mode\0\0"))
        {
            using RT = igl::opengl::ViewerCore::RotationType;
            auto new_type = static_cast<RT>(rotation_type);
            if (new_type != this->viewer->core().rotation_type)
            {
                if (new_type == RT::ROTATION_TYPE_NO_ROTATION)
                {
                    trackball_angle = this->viewer->core().trackball_angle;
                    orthographic = this->viewer->core().orthographic;
                    this->viewer->core().trackball_angle = Eigen::Quaternionf::Identity();
                    this->viewer->core().orthographic = true;
                }
                else if (this->viewer->core().rotation_type == RT::ROTATION_TYPE_NO_ROTATION)
                {
                    this->viewer->core().trackball_angle = trackball_angle;
                    this->viewer->core().orthographic = orthographic;
                }
                this->viewer->core().set_rotation_type(new_type);
            }
        }

        // Snap canonical view
        //ImGui::PushItemWidth(130 * this->gui->menu_scaling());
        //if (ImGui::Button("Snap canonical view"/*, ImVec2(-1, 0)*/))
        //{
        //    this->viewer->snap_to_canonical_quaternion();
        //}

        // Zoom
        ImGui::PushItemWidth(100 * this->gui->menu_scaling());
        //ImGui::DragFloat("Shininess", &(my_viewer.data().shininess), 0.05f, 0.0f, 100.0f);
        ImGui::DragFloat("Zoom", &(this->viewer->core().camera_zoom), 0.05f, 0.1f, 20.0f);

        // Orthographic view
        ImGui::Checkbox("Orthographic view", &(this->viewer->core().orthographic));

        // Background
        ImGui::ColorEdit4("Background", this->viewer->core().background_color.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

        // Line Color
        ImGui::ColorEdit4("Line color", this->viewer->data(this->viewer->selected_data_index).line_color.data(),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
        //ImGui::PopItemWidth();
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Geometry List", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (IglGeometry& geo : *(this->geoms))
        {
            size_t& geo_id = geo.id;
            bool& geo_show = geo.show;

            //make_checkbox(geo.name.c_str(), my_viewer.data().set_visible(geo_show, geo_id));

            if (ImGui::Checkbox(geo.name.c_str(), &(geo_show)))
            {
                this->viewer->selected_data_index = geo_id;
                this->viewer->data().set_visible(geo_show, this->viewer->core_list[0].id);
                //std::cout << "set " << geo.name << std::boolalpha << " " << geo_show << std::endl;
            }

            if (geo.name == "skel_map")
            {
                ImGui::SameLine();
                bool same = (geo.color_E.rows() == 1);
                if (ImGui::Checkbox("same color", &(same)))
                {
                    switch (same)
                    {
                    case true: // same color
                    {
                        geo.color_E.resize(1, 3);
                        geo.color_E << IglColor::green();
                        break;
                    }
                    case false: // random color
                    {
                        size_t v = geo.E(0, 0);
                        size_t rowE = geo.E.rows();
                        RVec_3d color = IglColor::random();
                        geo.color_E.resize(rowE, 3);

                        for (int row = 0; row < rowE; row++)
                        {
                            size_t my_v = geo.E(row, 0);
                            if (my_v != v)
                            {
                                v = my_v;
                                color = IglColor::random();
                            }

                            geo.color_E.row(row) << color;
                        }
                        break;
                    }
                    }

                    extern Display* display;
                    display->update();
                }
            }
        }
        //ImGui::PopItemWidth();
    }

    ImGui::Separator();

    // Set Parameter
    if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        extern Settings* settings;

        // Select rotation type
        int active_geo = 0;
        float active_line_width = settings->Viewer.line_width_skel;
        //float active_line_color = 1;
        static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
        static bool orthographic = true;
        ImGui::PushItemWidth(100 * this->gui->menu_scaling());
        if (ImGui::Combo("Set active geometry", &active_geo, "A\0B\000C\0\0"))
        {
            
        }

        ImGui::PushItemWidth(100 * this->gui->menu_scaling());
        ImGui::DragFloat("Line width", &this->viewer->data().line_width, 1.0f, 5.0f, 10.0f);
        //ImGui::PopItemWidth();
    }

    ImGui::End();
}

//GuiControl::
GuiControl::GuiControl()
{
    this->Reset();
}

void GuiControl::Draw(const char* title, bool* p_open)
{
    ImGui::SetNextWindowPos(ImVec2(205.0f, 260.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(195.0f, 500.0f), ImGuiCond_FirstUseEver);
    bool _viewer_menu_visible = true;

    ImGui::Begin(title, &_viewer_menu_visible);

    if (ImGui::Button("Reset            ", ImVec2(-1, 0)))
    {
        //extern size_t   process_nr;
        extern bool     settings_loaded;
        extern bool     is_triangle_mesh;

        extern Settings* settings;
        //extern GuiConsole* gui_console;

        extern Mesh* mesh;
        extern Triangle_mesh* tmesh;
        extern Skel* skel;
        extern SkelGraph* skel_graph;

        delete settings;
        delete gui_console;
        delete mesh;
        delete skel_graph;
        delete skel;

        process_nr = 0;
        settings_loaded = false;
        is_triangle_mesh = false;

        settings = new Settings();
        gui_console = new GuiConsole();

        mesh = new Mesh();
        tmesh = NULL;
        skel = new Skel();
        skel_graph = new SkelGraph();

        extern Display* display;
        display->erase(process_nr);
        Print("All caches have been cleared.\n");
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Load File", ImGuiTreeNodeFlags_DefaultOpen))
    {

        if (ImGui::Button("Load Settings      ", ImVec2(-1, 0)))
        {
            Print("[Button] Load Settings");

            extern Settings* settings;

            //extern size_t print_to;
            //extern size_t process_nr;
            extern bool   settings_loaded;

            if (process_nr == 0 && !settings_loaded)
            {
                settings->load(print_to);
                
                if (settings->File.name.length() > 0)
                {
                    settings_loaded = true;
                    process_nr = 1;
                    Print("\tdone.");
                }
                else
                {
                    settings_loaded = false;
                    process_nr = 0;
                    Print("[error] Failed! Can not load \"Settings.csv\"!");
                }

            }
            else
            {
                Print("If you want to reload file \"Settings.csv\", please click \"Reset\" button first.");
            }

            
        }

        if (ImGui::Button("Isotropic Remeshing", ImVec2(-1, 0)))
        {
            Print("\n");
            Print("[Button] Isotropic Remeshing");
            //extern size_t process_nr;
            if (process_nr >= 3)
            {
                Print("Please click \"Reset\" first!");
            }
            if (process_nr <= 0)
            {
                Print("Please load settings first!");
            }
            else if(process_nr == 1)
            {
                extern bool settings_loaded;
                if (settings_loaded)
                {
                    extern Settings* settings;  
                    std::ostringstream t1;
                    t1 << "\tRemeshing " << settings->Remeshing.remeshing_out_suffix << " ...";
                    Print(t1.str());
                    std::ostringstream t2;
                    t2 << "\t - target_edge_length = " << settings->Remeshing.remeshing_edge_length << "\n"
                        << "\t - number of iterations = " << settings->Remeshing.remeshing_nr_iter;
                    Print(t2.str());

                    
                    std::string in_name = settings->File.name;
                    std::string in_path = settings->root_path + settings->File.folder_path + in_name;
                    size_t idx_dot = in_path.rfind('.');

                    std::string out_path = in_path.substr(0, idx_dot);
                    size_t idx_seg = out_path.rfind("_res");
                    if (idx_seg != out_path.npos) { out_path = out_path.substr(0, idx_seg); }
                    out_path += settings->Remeshing.remeshing_out_suffix;

                    MeshFile::isotropic_remeshing(in_path, out_path,
                        settings->Remeshing.remeshing_edge_length, settings->Remeshing.remeshing_nr_iter);

                    settings->File.name = in_name.substr(0, in_name.rfind('.')) + settings->Remeshing.remeshing_out_suffix;
                    Print("\tdone.");
                }
            }
            else if (process_nr == 2)
            {
                extern Mesh* mesh;
                extern Display* display;
                extern Settings* settings;

                // reset loaded mesh -- similar to Reset
                Print("\tRemoving current loaded mesh.\n");

                process_nr = 1;
                delete mesh;
                mesh = NULL;
                mesh = new Mesh();
                display->erase(process_nr);
                Print("\tdone.");
                
                // isotropic remeshing -- same as process_nr == 1
                std::ostringstream t1;
                t1 << "\tRemeshing " << settings->Remeshing.remeshing_out_suffix << " ...";
                Print(t1.str());
                std::ostringstream t2;
                t2 << "\t - target_edge_length = " << settings->Remeshing.remeshing_edge_length << "\n"
                    << "\t - number of iterations = " << settings->Remeshing.remeshing_nr_iter;
                Print(t2.str());

                std::string in_name = settings->File.name;
                std::string in_path = settings->root_path + settings->File.folder_path + in_name;
                size_t idx_dot = in_path.rfind('.');

                std::string out_path = in_path.substr(0, idx_dot);
                size_t idx_seg = out_path.rfind("_res");
                if (idx_seg != out_path.npos) { out_path = out_path.substr(0, idx_seg); }
                out_path += settings->Remeshing.remeshing_out_suffix;

                MeshFile::isotropic_remeshing(in_path, out_path,
                    settings->Remeshing.remeshing_edge_length, settings->Remeshing.remeshing_nr_iter);

                settings->File.name = in_name.substr(0, in_name.rfind('.')) + settings->Remeshing.remeshing_out_suffix;
                Print("\tdone.");
                
                // reload mesh file -- same as Load Mesh File
                
                Print("\tLoading mesh file ... ");
                std::string path = settings->root_path + settings->File.folder_path + settings->File.name;
                mesh->load(path);

                if (mesh->is_triangle_mesh())
                {
                    extern bool is_triangle_mesh;
                    //extern Triangle_mesh* tmesh; 
                    extern Polyhedron* pmesh;
                    extern Display* display;

                    is_triangle_mesh = true;
                    process_nr = 2;

                    //tmesh = &(mesh->get_tmesh());
                    pmesh = &(mesh->get_pmesh());

                    display->insert(*pmesh);
                    display->update();
                }

                Print("\tdone.");
            }
        }

        if (ImGui::Button("Load Mesh File     ", ImVec2(-1, 0)))
        {
            Print("\n");
            Print("[Button] Load Mesh File");

            extern bool settings_loaded;
            //extern size_t process_nr;

            extern Settings* settings;
            extern Mesh* mesh;

            if (process_nr == 0)
            {
                Print("\tPlease load settings first!");
            }
            else if (process_nr >= 2)
            {
                Print("\tIf you want to reload mesh file, pleace click \"Reset\" button first!");
            }
            else if (process_nr == 1)
            {
                if (!settings_loaded)
                {
                    Print("The settings load was not successful, please modify parameters in \"Settings.csv\" manually!");
                }
                else
                {
                    Print("\tLoading mesh file ... ");

                    std::string path = settings->root_path + settings->File.folder_path + settings->File.name;
                    mesh->load(path);

                    if (mesh->is_triangle_mesh())
                    {
                        extern bool is_triangle_mesh;
                        //extern Triangle_mesh* tmesh; 
                        extern Polyhedron* pmesh; 
                        extern Display* display;

                        is_triangle_mesh = true;
                        process_nr = 2;

                        //tmesh = &(mesh->get_tmesh());
                        pmesh = &(mesh->get_pmesh());

                        display->insert(*pmesh);
                        display->update();
                    }

                    Print("\tdone.");
                }
            } // process_nr == 1
        } // if Button
    } // group "Load File"

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Skeletonization", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Extract Skeleton   ", ImVec2(-1, 0)))
        {
            Print("\n");
            Print("[Button] Extract Skeleton");

            //extern size_t   process_nr;
            extern Display* display;
            
            auto disp = display;
            size_t& pro_nr = process_nr;
            auto ins_skels = [disp, &pro_nr]()
            {
                extern Settings* settings;
                extern Mesh* mesh;
                extern Skel* skel;
                extern SkelGraph* skel_graph;

                skel->extract_to_end(*mesh);

                if (skel_graph != NULL)
                {
                    delete skel_graph;
                    skel_graph = NULL;
                }

                skel_graph = new SkelGraph(*skel, *mesh);
                skel_graph->output_skel_graph_to_files(settings);

                display->insert(*skel_graph);
                display->update();

                pro_nr = 3;
                Print("\tdone.");
            };

            if (process_nr >= 4)
            {
                Print("If you want to re-extract the skeleton, please click \"Reset\" first!");
            }
            else if (process_nr <= 0)
            {
                Print("Please load settings first!");
            }
            else if (process_nr == 1)
            {
                Print("Please load mesh file first!");
            }
            else if (process_nr == 3)
            {
                Print("\tRe-extracting skeletons ...");
                Print("\t\tclear all existing skeletons ...");
                display->erase(process_nr);
                Print("\t\tdone.");

                ins_skels();
            }
            else if (process_nr == 2)
            {
                ins_skels();
            }
        }

        if (ImGui::Button("Extract Segments", ImVec2(-1, 0)))
        {
            Print("[Button] Extract Segments");

            //extern size_t process_nr;
            extern Display* display;

            auto ins_segs = []()
            {
                extern Settings*     settings;
                //extern Skel*         skel;
                extern Polyhedron*   pmesh;
                extern SkelGraph*    skel_graph;
                extern Segmentation* segmentation;

                //Skeleton& skeleton = skel->get_skeleton();
                //std::string path = settings->root_path + settings->File.folder_path + settings->File.name;

                segmentation->set_segments(*skel_graph, *pmesh);
                display->insert(segmentation->get_segments());
                display->update();

                process_nr = 4;
            };

            switch (process_nr)
            {
            case 0:
            {
                Print("Please load settings first!");
                break;
            }
            case 1:
            {
                Print("Please load mesh file first!");
                break;
            }
            case 2:
            {
                Print("Please extract skeleton first!");
                break;
            }
            case 3:
            {
                Print("\tRe-caculating segments ... ");

                ins_segs();
                
                Print("\t\tdone");
                break;
            }
            case 4:
            {
                Print("\tRe-caculating segments ... ");
                Print("\t\tclear all existing segments ... ");
                display->erase(process_nr);

                ins_segs();

                Print("\t\tdone");
                break;
            }
            default:
            {
                Print("\tPlease click \"Reset\" button first! ");
            }
            }

            
        }
    } // group "skeletonization"

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Slicing", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Get Next Stitch   ", ImVec2(-1, 0)))
        {
            Print("[Button] Get Next Stitch");
            std::cout << "" << std::endl;
        }

        if (ImGui::Button("Get Next layer    ", ImVec2(-1, 0)))
        {
            Print("[Button] Get Next layer");
            std::cout << "" << std::endl;
        }

        if (ImGui::Button("Slice To End      ", ImVec2(-1, 0)))
        {
            Print("[Button] Slice To End");
            std::cout << "" << std::endl;
        }
    }

    ImGui::Separator();

    // Set Parameter
    if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
    {
        extern Settings* settings;
        // set para
        ImGui::PushItemWidth(60);
        ImGui::InputDouble("stitch_width ", &(settings->Stitch.stitch_width), 0, 0, "%.3f", 0);
        ImGui::InputDouble("stitch_height", &(settings->Stitch.stitch_height), 0, 0, "%.3f", 0);
        ImGui::InputDouble("obj_scale      ", &(settings->Stitch.obj_scale), 0, 0, "%.3f", 0);
        ImGui::InputDouble("layer0_offset", &(settings->Stitch.layer0_offset), 0, 0, "%.3f", 0);
        ImGui::PopItemWidth();
    }


    ImGui::End();
}




// Display::
Display::Display()                                  { }
Display::Display(std::string path)                  { this->insert(path); }
Display::Display(IglGeometry geometry)              { this->insert(geometry); }

Display::Display(Triangle_mesh& tmesh)              { this->insert_tmesh(tmesh); }
Display::Display(Skeleton& skeleton)                { this->insert_skeleton(skeleton); }
Display::Display(Skel& skel)                        { this->insert_skel(skel); }
Display::Display(SkelGraph& skel_graph)             { this->insert_skel_graph(skel_graph); }
Display::Display(Segment& segment)                  { this->insert_segment(segment); }
Display::Display(std::vector<Segment>& segments)    { this->insert_segments(segments); }
Display::Display(Stitches& stitches)                { this->insert_stitches(stitches); }

Display::Display(MAT_3d& V, MAT_2i& E, MAT_3i& F) { this->insert(V, E, F); }
Display::Display(MAT_3d& V, MAT_3i& F) { this->insert(V, F); }
Display::Display(MAT_3d& V, MAT_2i& E) { this->insert(V, E); }
Display::Display(MAT_3d& V, MAT_2i& E, MAT_3i& F, RVec_3d color_V, RVec_3d color_E, RVec_3d color_F)
{
    this->insert(V, E, F, color_V, color_E, color_F);
}
Display::Display(MAT_3d& V, MAT_3i& F, RVec_3d color_V, RVec_3d color_F)
{
    this->insert(V, F, color_V, color_F);
}
Display::Display(MAT_3d& V, MAT_2i& E, RVec_3d color_V, RVec_3d color_E)
{
    this->insert(V, E, color_V, color_E);
}

// public
void Display::erase(size_t& process_nr)
{
    std::vector<std::string> key_names = { "mesh", "skel", "segment"};
    std::vector<size_t> idxs;

    switch (process_nr)
    {
    case 2: // erase tmesh, skel x3
    {
        idxs.push_back(0);
        idxs.push_back(1);
        idxs.push_back(2);
        break;
    }
    case 3: // erase skel x3
    {
        idxs.push_back(1);
        idxs.push_back(2);
        break;
    }
    case 4: // erase segment xN
    {
        idxs.push_back(2);
        break;
    }
    default:
    {
        for (size_t i = 0; i < key_names.size(); i++)
        {
            idxs.push_back(i);
        }
        break;
    }
    } // switch

    for (size_t i : idxs)
    {
        std::string& key_name = key_names[i];

        size_t offset = 0;
        while (true)
        {
            auto& geoms = this->igl_geoms;

            size_t numGeo = geoms.size();
            if (offset >= numGeo) { break; }

            auto it_geoms_begin = geoms.begin();
            auto it_geoms_end   = geoms.end();
            auto it_geoms       = it_geoms_begin + offset;

            std::string& geo_name = it_geoms->name;
            auto pos = geo_name.find(key_name);
            if (pos != geo_name.npos)
            {
                geoms.erase(it_geoms);
            }
            else
            {
                offset++;
            }
        } // while
    } // for

    this->update();

    return;
}

void Display::update()
{
    size_t id_max = this->viewer.data_list.size() - 1;
    for (; id_max > 0; --id_max)
    {
        this->viewer.erase_mesh(id_max);
    }
    this->viewer.data().clear();

    size_t idx = 0;
    for (IglGeometry& my_geom : this->igl_geoms)
    {
        size_t numV = my_geom.V.rows();
        size_t numE = my_geom.E.rows();
        size_t numF = my_geom.F.rows();

        if (numV <= 0) continue;

        if (idx > 0)
        {
            this->viewer.append_mesh();
        }
        this->viewer.data().clear();
        set_parameter(my_geom.name);

        if (numE > 0 && numF == 0)
        {
            //this->viewer.data().set_vertices(my_geom.V);
            this->viewer.data().set_edges(my_geom.V, my_geom.E, my_geom.color_E);

            //MAT_3d colors_V;
            //my_geom.get_colors_V(colors_V);
            //this->viewer.data().set_colors(colors_V);
        }
        else if (numE == 0 && numF > 0)
        {
            this->viewer.data().set_mesh(my_geom.V, my_geom.F);

            MAT_3d colors_V;
            MAT_3d colors_F;
            my_geom.get_colors_V(colors_V);
            my_geom.get_colors_F(colors_F);
            this->viewer.data().set_colors(colors_V);
            this->viewer.data().set_colors(colors_F);
        }
        else if (numE > 0 && numF > 0)
        {
            this->viewer.data().set_mesh(my_geom.V, my_geom.F);

            this->viewer.data().set_edges(my_geom.V, my_geom.E, my_geom.color_E);

            MAT_3d colors_V;
            MAT_3d colors_F;
            my_geom.get_colors_V(colors_V);
            my_geom.get_colors_F(colors_F);
            this->viewer.data().set_colors(colors_V);
            this->viewer.data().set_colors(colors_F);
        }
        else
        {
            this->viewer.data().set_vertices(my_geom.V);

            MAT_3d colors_V;
            my_geom.get_colors_V(colors_V);
            this->viewer.data().set_colors(colors_V);
        }

        idx++;
    }

    //this->viewer.data().show_overlay_depth = false;
    this->set_camera_zoom();
}


// private
void Display::insert_obj(std::string path)
{
    std::ifstream input;
    input.open(path, std::ios::in);

    if (!input.is_open())
    {
        std::cout << "Cannot open file \"" << path << "\"!";
        return;
    }

    std::string line = "";
    std::vector<std::string>         buf;
    std::vector<std::vector<double>> v;
    std::vector<std::vector<int>>    e;
    std::vector<std::vector<int>>    f;
    
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

    IglGeometry my_geom;    
    my_geom.name = path.substr(path.rfind("/"));
    my_geom.id = this->igl_geoms.size();

    MAT_3d& my_V = my_geom.V;
    MAT_2i& my_E = my_geom.E;
    MAT_3i& my_F = my_geom.F;

    if (numV > 0)
    {
        my_geom.color_V = IglColor::black();

        my_V.resize(numV, 3);
        for (int i = 0; i < numV; i++)
        {
            my_V.row(i) << v[i][0], v[i][1], v[i][2];
        }
    }
    if (numE > 0)
    {
        my_geom.color_E = IglColor::black();

        my_E.resize(numE, 2);
        for (int i = 0; i < numE; i++)
        {
            my_E.row(i) << e[i][0], e[i][1];
        }
    }
    if (numF > 0)
    {
        my_geom.color_F = IglColor::yellow();

        my_F.resize(numF, 3);
        for (int i = 0; i < numF; i++)
        {
            my_F.row(i) << f[i][0], f[i][1], f[i][2];
        }
    }

    this->insert_geometry(my_geom);
}
void Display::insert_off(std::string path)
{
    IglGeometry my_geom; 
    my_geom.name    = path.substr(path.rfind("/"));
    my_geom.id      = this->igl_geoms.size();

    my_geom.color_V = IglColor::white();
    my_geom.color_F = IglColor::yellow();

    MAT_3d& my_V    = my_geom.V;
    MAT_3i& my_F    = my_geom.F;

    igl::readOFF(path, my_V, my_F);
    this->insert_geometry(my_geom);
}
void Display::insert_stl(std::string path)
{
    std::ifstream input(path);

    IglGeometry my_geom;
    my_geom.name    = path.substr(path.rfind("/"));
    my_geom.id      = this->igl_geoms.size();

    my_geom.color_V = IglColor::black();
    my_geom.color_F = IglColor::yellow();

    MAT_3d& my_V    = my_geom.V;
    MAT_3i& my_F    = my_geom.F;
    MAT_3d N;

    igl::readSTL(input, my_V, my_F, N);
    this->insert_geometry(my_geom);
}

void Display::insert_skel_graph(SkelGraph& skel_graph)
{
    const std::vector<SkelNode>& skel_nodes = skel_graph.get_skel_nodes();
    const std::vector<SkelEdge>& skel_edges = skel_graph.get_skel_edges();
    const std::vector<SkelExtn>& skel_extns = skel_graph.get_skel_extns();
    

    const auto ins_skeleton = [&skel_nodes, &skel_edges, this]()
    {
        size_t numV = skel_nodes.size();
        size_t numE = skel_edges.size();

        MAT_3d my_V;
        MAT_2i my_E;

        my_V.resize(numV, 3);
        my_E.resize(numE, 2);

        size_t v_i = 0;
        for (SkelNode nd : skel_nodes)
        {
            const Point& p = nd.point();

            my_V.row(v_i) << p.x(), p.y(), p.z();

            v_i++;
        }
        size_t e_i = 0;
        for (SkelEdge eg : skel_edges)
        {
            my_E.row(e_i) << eg.source() - 1, eg.target() - 1;

            e_i++;
        }

        size_t id = this->igl_geoms.size();
        return IglGeometry("skel", id, my_V, my_E, IglColor::yellow(), IglColor::yellow());
    };
    const auto ins_extension = [&skel_extns, this]()
    {
        MAT_3d my_V;
        MAT_2i my_E;

        size_t numE = skel_extns.size();
        size_t numV = 2 * numE;

        my_V.resize(numV, 3);
        my_E.resize(numE, 2);

        size_t v_i = 0;
        size_t e_i = 0;

        for (SkelExtn my_extn : skel_extns)
        {
            const Point& top_node   = my_extn.get_skel_node().point();
            const Point& extn_point = my_extn.get_extn_point();

            my_V.row(v_i)     << top_node.x(),   top_node.y(),   top_node.z();
            my_V.row(v_i + 1) << extn_point.x(), extn_point.y(), extn_point.z();
            my_E.row(e_i)     << v_i,            v_i + 1;

            v_i += 2;
            e_i++;
        }

        size_t id = this->igl_geoms.size();
        return IglGeometry("skel_ext", id, my_V, my_E, IglColor::red(), IglColor::red());
    };
    const auto ins_map_p = [&skel_nodes, this]()
    {
        MAT_3d my_V;
        MAT_2i my_E;

        size_t numV = 0;
        size_t numE = 0;

        size_t v_i = 0;
        size_t e_i = 0;

        MAT_3d color_E;

        for (auto& my_nd : skel_nodes)
        {
            auto& skel_p     = my_nd.point();
            auto& map_points = my_nd.get_mapping_vertices();

            numV += (map_points.size() + 1);
            numE += map_points.size();

            my_V.conservativeResize(numV, 3);
            my_E.conservativeResize(numE, 2);
            color_E.conservativeResize(numE, 3);
            RVec_3d my_color = IglColor::random();

            my_V.row(v_i) << skel_p.x(), skel_p.y(), skel_p.z();
            size_t i = v_i;
            v_i++;
            for (auto& map_p : map_points)
            {
                auto& p = map_p.second;
                my_V.row(v_i)    << p.x(), p.y(), p.z();
                my_E.row(e_i)    << i,     v_i;
                color_E.row(e_i) << my_color;

                v_i++;
                e_i++;
            }
        }
        
        size_t id = this->igl_geoms.size();
        return IglGeometry("skel_map", id, my_V, my_E, IglColor::green(), color_E);
    };

    this->insert_geometry(ins_skeleton());
    this->insert_geometry(ins_extension());
    this->insert_geometry(ins_map_p());
}
void Display::insert_tmesh(Triangle_mesh& tmesh)
{
    int numV = tmesh.number_of_vertices();
    int numF = tmesh.number_of_faces();

    MAT_3d my_V;
    MAT_3i my_F;

    my_V.resize(numV, 3);
    my_F.resize(numF, 3);


    // add vertices
    size_t v_i = 0;
    for (const auto& v : tmesh.vertices())
    {
        const Point& p = tmesh.point(v);
        my_V.row(v_i) << p.x(), p.y(), p.z();
        v_i++;
    }

    // add facets
    size_t f_i = 0;
    for (const auto& fd : tmesh.faces())
    {
        size_t axis = 0;
        for (const auto& vd : CGAL::vertices_around_face(tmesh.halfedge(fd), tmesh))
        {
            my_F(f_i, axis) = vd;
            axis++;
        }
        //std::cout << "face " << f_i << ": " 
        //          << (this->F)(f_i, 0) << "\t"
        //          << (this->F)(f_i, 1) << "\t"
        //          << (this->F)(f_i, 2) << "\t"
        //          << std::endl;
        f_i++;
    }

    size_t id = this->igl_geoms.size();

    IglGeometry my_geom("mesh", id, my_V, my_F, IglColor::white(), IglColor::yellow());
    my_geom.color_E = IglColor::white();
    this->insert_geometry(my_geom);
}
void Display::insert_pmesh(Polyhedron& pmesh)
{
    MAT_3d my_V;
    MAT_3i my_F;
    Mesh::get_VF(my_V, my_F, pmesh);

    size_t id = this->igl_geoms.size();

    IglGeometry my_geom("mesh", id, my_V, my_F, IglColor::white(), IglColor::yellow());
    my_geom.color_E = IglColor::white();
    this->insert_geometry(my_geom);
}

void Display::insert_skeleton(Skeleton& skeleton)
{
    auto& my_nodes = skeleton.m_vertices;  // skeleton nodes with associated mesh vertices
    auto& my_edges = skeleton.m_edges;

    size_t numV = my_nodes.size();
    size_t numE = my_edges.size();

    MAT_3d my_V;
    MAT_2i my_E;

    my_V.resize(numV, 3);
    my_E.resize(numE, 2);

    auto it_nd_begin = my_nodes.begin();
    auto it_nd_end   = my_nodes.end();
    auto it_nd       = it_nd_begin;

    size_t v_i = 0;
    for (; it_nd != it_nd_end; it_nd++)
    {
        const Point& my_point = it_nd->m_property.point;

        my_V.row(v_i) << my_point.x(), my_point.y(), my_point.z();

        v_i++;
    }

    auto it_eg_begin = my_edges.begin();
    auto it_eg_end   = my_edges.end();
    auto it_eg       = it_eg_begin;

    size_t e_i = 0;
    for (; it_eg != it_eg_end; it_eg++)
    {
        my_E.row(e_i) << it_eg->m_source, it_eg->m_target;
        e_i++;
    }

    size_t id = this->igl_geoms.size();
    IglGeometry my_geom("skel", id, my_V, my_E, IglColor::black(), IglColor::yellow());
    this->insert_geometry(my_geom);

}
void Display::insert_skel(Skel& skel)
{
    Skeleton& my_skeleton = skel.get_skeleton();
    this->insert_skeleton(my_skeleton);
}
void Display::insert_segment(Segment& segment)
{
    MAT_3d my_V = segment.get_V();
    MAT_3i my_F = segment.get_F();
    size_t id = this->igl_geoms.size();
    std::ostringstream label;
    label << "segment_" << segment.get_id();

    IglGeometry my_geom(label.str(), id, my_V, my_F, IglColor::olive(), IglColor::random());
    this->insert_geometry(my_geom);
}
void Display::insert_segments(std::vector<Segment>& segments)
{
    for (Segment& segment : segments)
    {
        this->insert_segment(segment);
    }
}

void Display::insert_stitches(Stitches& stitches)
{
    insert_stitches(stitches, 0, 0);
}
void Display::insert_stitches(Stitches& stitches, size_t pos)
{
    insert_stitches(stitches, pos, 0);
}
void Display::insert_stitches(Stitches& stitches, size_t pos, size_t len)
{
    // too old, need update




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

        V.row(idx) << v(0), v(1), v(2);

        viewer.data().set_vertices(v/*, color_v */);
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


    for (; it_stitches != it_line_end; it_stitches++)
    {
        auto& idx_last = it_last_stitch->index;
        auto& idx = it_stitches->index;

        if (idx == 0) { continue; }
        else
        {
            const auto& it_source = V.row(idx_last);
            const auto& it_target = V.row(idx);

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
            const auto& it_source = V.row(in_0);
            const auto& it_target = V.row(idx);
            const auto& color_eg = IglColor::green();

            viewer.data().add_edges(it_source, it_target, color_eg);
            //std::cout << "E " << in_0 << " -> " << idx << std::endl;

            break;
        }
        case 'd':
        {
            uint32_t in_0 = it_stitches->in_0;
            uint32_t in_1 = it_stitches->in_1;

            const auto& it_source_0 = V.row(in_0);
            const auto& it_source_1 = V.row(in_1);
            const auto& it_target = V.row(idx);
            const auto& color_eg = IglColor::green();

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


void Display::set_parameter(const std::string& geom_name)
{
    extern Settings* settings;

    if (geom_name == "mesh")
    {
        this->viewer.data().point_size = settings->Viewer.point_size_mesh;
        this->viewer.data().line_width = settings->Viewer.line_width_mesh;
    }
    else if (geom_name == "skel")
    {
        this->viewer.data().point_size = settings->Viewer.point_size_skel;
        this->viewer.data().line_width = settings->Viewer.line_width_skel;
    }
    else if (geom_name == "skel_ext")
    {
        this->viewer.data().point_size = settings->Viewer.point_size_skel_ext;
        this->viewer.data().line_width = settings->Viewer.line_width_skel_ext;
    }
    else if (geom_name == "skel_map")
    {
        this->viewer.data().point_size = settings->Viewer.point_size_skel_map;
        this->viewer.data().line_width = settings->Viewer.line_width_skel_map;
    }
    else if (geom_name == "stitch")
    {
        this->viewer.data().point_size = settings->Viewer.point_size_stitch;
        this->viewer.data().line_width = settings->Viewer.line_width_stitch;
    }
    else
    {
        this->viewer.data().point_size = settings->Viewer.point_size_default;
        this->viewer.data().line_width = settings->Viewer.line_width_default;
    }
}

void Display::set_camera_zoom()
{
    if (igl_geoms.size() <= 0) { return; }

    Bound& my_bound = this->bound;
    my_bound.set(this->igl_geoms);
    RVec_3d my_size = my_bound.get_xyz_size();
    double volume = my_size(0) * my_size(1) * my_size(2);

    //std::ostringstream text;
    //text << "\nBounding box \t\t" << "volumen = " << volume << "\n"
    //    << "\tx = " << my_size(0) << "\ty = " << my_size(1) << "\tz = " << my_size(2) << "\t";
    //Print(text.str());

    MAT_3d V;
    my_bound.get_bounding_box_V(V);
    this->viewer.core().align_camera_center(V);
    this->viewer.core().set_rotation_type(IGL_ViewerCore::RotationType::ROTATION_TYPE_TRACKBALL);
}

void Display::set_gui()
{
    auto& my_gui = this->gui;
    auto& my_viewer = this->viewer;

    my_viewer.plugins.push_back(&my_gui);

    bool show_gui_console = true;
    bool show_gui_display = true;
    bool show_gui_control = true;

    //Add content to current menu window
    my_gui.callback_draw_viewer_window = [&]()
    {
        //extern size_t print_to;
        if (print_to == 1)
        {
            //extern GuiConsole* gui_console;
            //static GuiConsole& my_console = *gui_console;
            gui_console->Draw("Console", &show_gui_console);
        }
        
        static GuiDisplay gui_display(&my_viewer, &my_gui, &(this->igl_geoms));
        gui_display.Draw("Display", &show_gui_display);

        static GuiControl gui_control;
        gui_control.Draw("Process Control", &show_gui_control);
    };
}

void Display::set_events()
{
    this->viewer.callback_key_down = &Display::key_down;
}

bool Display::key_down(IGL_Viewer& viewer, unsigned char key, int modifier)
{
    std::cout << "Key: " << key << " " << (unsigned int)key << std::endl;
    if (key == '1')
    {
        //viewer.data().clear();
    }

    return false;
}

END_PROJECT_NAMESPACE