/*
  Main source file.
*/

#include <iostream>
#include "algebra.h"
#include "block.h"
//#include "chessboard.h"

void renderCubes()
{
    using namespace Imager;

    Scene scene1(Color(0.0, 0.0, 0.0));  

    Cuboid *cuboid1 = new Cuboid(2.0, 2.0, 2.0);  //cube with length, width and height
    cuboid1->SetFullMatte(Color(0.7, 0.7, 0.8));
    cuboid1->Move(0.0, 0.0, -50.0);
    cuboid1->RotateX(-115.0);
    cuboid1->RotateY(22.0);

    scene1.AddSolidObject(cuboid1);
    scene1.AddLightSource(LightSource(Vector(-5.0, 50.0, +20.0), Color(0.7, 0.7, 0.7)));
    
    const char *filename1 = "../output/cuboid_1.png";
    scene1.SaveImage(filename1, 300, 300, 3.0, 2);
    //std::cout << "Wrote cube" << filename << std::endl;
    
    Scene scene2(Color(0.0, 0.0, 0.0));

    Cuboid *cuboid2 = new Cuboid(2.0, 2.0, 2.0);
    cuboid2->SetFullMatte(Color(0.7, 0.7, 0.8));
    cuboid2->Move(0.0, 0.0, -50.0);
    cuboid2->RotateX(-115.0);
    cuboid2->RotateY(-22.0);

    scene2.AddSolidObject(cuboid2);
    scene2.AddLightSource(LightSource(Vector(-5.0, 50.0, +20.0), Color(0.7, 0.7, 0.7)));

    const char *filename2 = "../output/cuboid_2.png";
    scene2.SaveImage(filename2, 300, 300, 3.0, 2);
    //std::cout << "Wrote cube" << filename << std::endl;

    Scene scene3(Color(0.0, 0.0, 0.0));

    Cuboid *cuboid3 = new Cuboid(2.0, 2.0, 2.0);
    cuboid3->SetFullMatte(Color(0.7, 0.7, 0.8));
    cuboid3->Move(0.0, 0.0, -20.0);
    cuboid3->RotateX(-90.0);
    cuboid3->RotateY(22.0);

    scene3.AddSolidObject(cuboid3);
    scene3.AddLightSource(LightSource(Vector(-5.0, 50.0, +20.0), Color(0.7, 0.7, 0.7)));
    
    const char *filename3 = "../output/cuboid_3.png";
    scene3.SaveImage(filename3, 300, 300, 3.0, 2);
    //std::cout << "Wrote cube" << filename << std::endl;
    
    Scene scene4(Color(0.0, 0.0, 0.0));

    Cuboid *cuboid4 = new Cuboid(2.0, 2.0, 2.0);
    cuboid4->SetFullMatte(Color(0.7, 0.7, 0.8));
    cuboid4->Move(0.0, 0.0, -55.0);
    cuboid4->RotateX(-115.0);
    cuboid4->RotateY(122.0);

    scene4.AddSolidObject(cuboid4);
    scene4.AddLightSource(LightSource(Vector(-5.0, 50.0, +20.0), Color(0.7, 0.7, 0.7)));

    const char *filename4 = "../output/cuboid_4.png";
    scene4.SaveImage(filename4, 300, 300, 3.0, 2);
    //std::cout << "Wrote cube" << filename << std::endl;

    Scene scene5(Color(0.0, 0.0, 0.0));

    Cuboid *cuboid5 = new Cuboid(2.0, 2.0, 2.0);
    cuboid5->SetFullMatte(Color(0.7, 0.7, 0.8));
    cuboid5->Move(0.0, 0.0, -20.0);
    cuboid5->RotateX(21.0);
    cuboid5->RotateY(22.0);

    scene5.AddSolidObject(cuboid5);
    scene5.AddLightSource(LightSource(Vector(-5.0, 50.0, +20.0), Color(0.7, 0.7, 0.7)));
    
    const char *filename5 = "../output/cuboid_5.png";
    scene5.SaveImage(filename5, 300, 300, 3.0, 2);
    //std::cout << "Wrote cube" << filename << std::endl;
    
    Scene scene6(Color(0.0, 0.0, 0.0));

    Cuboid *cuboid6 = new Cuboid(2.0, 2.0, 2.0);
    cuboid6->SetFullMatte(Color(0.7, 0.7, 0.8));
    cuboid6->Move(0.0, 0.0, -65.0);
    cuboid6->RotateX(-35.0);
    cuboid6->RotateY(122.0);

    scene6.AddSolidObject(cuboid6);
    scene6.AddLightSource(LightSource(Vector(-5.0, 50.0, +20.0), Color(0.7, 0.7, 0.7)));

    const char *filename6 = "../output/cuboid_6.png";
    scene6.SaveImage(filename6, 300, 300, 3.0, 2);
    //std::cout << "Wrote cube" << filename << std::endl;
}


typedef void (* COMMAND_FUNCTION) ();

struct CommandEntry
{
    const char* verb;           // the command line option
    COMMAND_FUNCTION command;   // function to call when option encountered
    const char* help;           // usage text that explains the option
};


const CommandEntry CommandTable[] =
{
    { "run", renderCubes,
        "    Runs renderCubes() that generates 6 different images of cubes.\n"
    },

};

const size_t NUM_COMMANDS = sizeof(CommandTable) / sizeof(CommandTable[0]);


void PrintUsageText()
{
    using namespace std;

    cout <<
        "\n"
        "The following command line options are supported:\n";

    for (size_t i=0; i < NUM_COMMANDS; ++i)
    {
        cout << "\n";
        cout << CommandTable[i].verb << "\n";
        cout << CommandTable[i].help;
    }

    cout << endl;
}


int main(int argc, const char *argv[])
{
    using namespace std;

    int rc = 1;

    if (argc == 1)
    {

        PrintUsageText();
    }
    else
    {

        const string verb = argv[1];
        bool found = false;
        for (size_t i=0; i < NUM_COMMANDS; ++i)
        {
            if (verb == CommandTable[i].verb)
            {
                found = true;                   
                CommandTable[i].command();      
                rc = 0;                         
                break;                          
            }
        }

        if (!found)
        {
            cerr << "ERROR:  Unknown command line option '" << verb << "'" << endl;
        }
    }
    
    void renderCubes();

    return rc;
}
