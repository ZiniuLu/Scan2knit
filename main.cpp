/* Created on: 10/03/2021
 *     Author: Ziniu Lu (luziniuoskar@outlook.com)
 */

#include "CurveSkeleton.h"
#include "MeshFile.h"

using namespace PROJECT_NAMESPACE;
using namespace std;


int main(int argc, char *argv[])
{
    std::cout.flush();
    // 1. open file
    std::string filePath = "../data/hand.off";
    MeshFile meshFile((argc > 1) ? argv[1] : filePath.c_str());

    if (!meshFile.is_open())
    {
        std::cout << "Can not open file: " << meshFile.getFileName() << " !" << std::endl;
        return EXIT_FAILURE;
    }
        

    // 2. extract skeleton
    //CurveSkeleton* p_skel_1 = NULL;
    //p_skel_1 = new CurveSkeleton(meshFile);
    //p_skel_1->extract_to_end(false);

    CurveSkeleton* p_skel = NULL;
    p_skel = new CurveSkeleton(meshFile);
    p_skel->extract_to_end(true);








    delete p_skel;

    return EXIT_SUCCESS;
}