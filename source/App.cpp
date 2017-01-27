/** \file App.cpp */
#include "App.h"

// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    {
        G3DSpecification g3dSpec;
        g3dSpec.audio = false;
        initGLG3D(g3dSpec);
    }

    GApp::Settings settings(argc, argv);

    // Change the window and other startup parameters by modifying the
    // settings class.  For example:
    settings.window.caption             = argv[0];

    // Set enable to catch more OpenGL errors
    // settings.window.debugContext     = true;

    // Some common resolutions:
    // settings.window.width            =  854; settings.window.height       = 480;
    // settings.window.width            = 1024; settings.window.height       = 768;
    settings.window.width               = 1280; settings.window.height       = 720;
    //settings.window.width             = 1920; settings.window.height       = 1080;
    // settings.window.width            = OSWindow::primaryDisplayWindowSize().x; settings.window.height = OSWindow::primaryDisplayWindowSize().y;
    settings.window.fullScreen          = false;
    settings.window.resizable           = ! settings.window.fullScreen;
    settings.window.framed              = ! settings.window.fullScreen;

    // Set to true for a significant performance boost if your app can't render at 60fps, or if
    // you *want* to render faster than the display.
    settings.window.asynchronous        = false;

    settings.hdrFramebuffer.depthGuardBandThickness = Vector2int16(64, 64);
    settings.hdrFramebuffer.colorGuardBandThickness = Vector2int16(0, 0);
    settings.dataDir                    = FileSystem::currentDirectory();
    settings.screenshotDirectory        = "../journal/";

    settings.renderer.deferredShading = true;
    settings.renderer.orderIndependentTransparency = false;

    return App(settings).run();
}

//Creates a sphere-like object for my custom scene
void App::createBall() {
 int verticies = 100;
 TextOutput output ("model/ball.off");

 String header = "OFF\n";
 header += (String) std::to_string((verticies) + 2) + " " + (String) std::to_string((verticies*2)) + " 0\n\n";

 String vertexArray = "";

 for(float i = 0; i < verticies; ++i) {
    float a = ((float) 2.0) * pif()*i/(((float)verticies));
    float z = cos(a) * 1;
    float x = sin(a) * 1;
    vertexArray += (String) std::to_string(x) + " 0 " + (String) std::to_string(z) + "\n";
 }
 vertexArray += "0 1 0\n0 -1 0\n";

 String edgeArray = "";
 for(int i = 0; i < verticies; ++i) {
    edgeArray += "3 " + (String) std::to_string(verticies) + " "+ (String) std::to_string(i) +" "+ (String) std::to_string((i + 1) % verticies) +"\n";
    edgeArray += "3 " + (String) std::to_string(verticies + 1) + " "+ (String) std::to_string(i) +" "+ (String) std::to_string((i + 1) % verticies) +"\n";
 }

 output.writeSymbol(header + vertexArray + edgeArray);
 output.commit(true);
}

//Returns a String that is the contents of an off file that corralates to a cylinder of a given radius and height
String App::createCylinder(float radius, float height) {
 int verticies = 100;

 String header = "OFF\n";
 header += (String) std::to_string((verticies*2) + 2) + " " + (String) std::to_string(verticies*3) + " 0\n\n";

 String vertexArray = "";

 for(float i = 0; i < verticies; ++i) {
    float a = ((float) 2.0) * pif()*i/(((float)verticies));
    float z = cos(a) * radius;
    float x = sin(a) * radius;
    vertexArray += (String) std::to_string(x) + " " + (String) std::to_string(-(height/2)) + " " + (String) std::to_string(z) + "\n";
 }

 for(float i = 0; i < verticies; ++i) {
    float a = ((float) 2.0) * pif()*i/(((float)verticies));
    float z = cos(a) * radius;
    float x = sin(a) * radius;
    vertexArray += (String) std::to_string(x) + " " + (String) std::to_string(height/2) + " " + (String) std::to_string(z) + "\n";
 }
 vertexArray += "0 " + (String) std::to_string(-(height/2)) + " 0\n0 " + (String) std::to_string(height/2) + " 0\n\n";

 String edgeArray = "";
 for(int i = 0; i < verticies; ++i) {
    edgeArray += "3 " + (String) std::to_string(verticies*2) + " "+ (String) std::to_string(i) +" "+ (String) std::to_string((i + 1) % verticies) +"\n";
 }

 for(int i = verticies; i < verticies*2; ++i) {
    edgeArray += "3 " + (String) std::to_string((verticies*2) + 1) + " "+ (String) std::to_string(i) +" "+ (String) std::to_string(((i + 1) % verticies) + verticies) +"\n";
 }

for(int i = 0; i < verticies; ++i) {
    edgeArray += "4 " + (String) std::to_string(i) + " "+ (String) std::to_string((i + 1) % verticies) +" "+ (String) std::to_string(((i + 1) % verticies) + verticies) + " " + (String) std::to_string(verticies + i) +"\n";
 }

 return header + vertexArray + edgeArray;
}

//Creates a GUI that allows a user to generate a cylinder in an off file of a given radius and height
void App::makeCylinder() {

    GuiPane* cylinderPane = debugPane->addPane("Cylinder");

    cylinderPane->setNewChildSize(240);
    cylinderPane->addNumberBox("Radius", &m_cylinderRadius, "m", 
        GuiTheme::LOG_SLIDER, 1.0f, 100.0f)->setUnitsSize(30);
        
    cylinderPane->addNumberBox("Height", &m_cylinderHeight, "m/px", 
        GuiTheme::LOG_SLIDER, 1.0f, 100.0f)->setUnitsSize(30);
    
    cylinderPane->addButton("Generate", [this](){
        drawMessage("Generating Cylinder.");
        TextOutput output ("model/cylinder.off");
        String cylinder = createCylinder(m_cylinderRadius, m_cylinderHeight);

        output.writeSymbol(cylinder);
        output.commit(true);
        G3D::ArticulatedModel::clearCache();
    });
}

//Creates a GUI that allows a user to generate a heightfield with a given xz and y scale based on a given image
void App::makeHeightfield() {

    GuiPane* heightfieldPane = debugPane->addPane("Heightfield");

    heightfieldPane->setNewChildSize(240);
    heightfieldPane->addNumberBox("Max Y", &m_heightfieldYScale, "m", 
        GuiTheme::LOG_SLIDER, 0.0f, 100.0f)->setUnitsSize(30);
        
    heightfieldPane->addNumberBox("XZ Scale", &m_heightfieldXZScale, "m/px", 
        GuiTheme::LOG_SLIDER, 0.001f, 10.0f)->setUnitsSize(30);
 
    heightfieldPane->beginRow(); {
        heightfieldPane->addTextBox("Input Image", &m_heightfieldSource)->setWidth(210);
        heightfieldPane->addButton("...", [this]() {
            FileDialog::getFilename(m_heightfieldSource, "png", false);
        })->setWidth(30);
    } heightfieldPane->endRow();
    
    heightfieldPane->addButton("Generate", [this](){
        shared_ptr<G3D::Image> image;
        try {
            drawMessage("Generating Heightfield.");
            image = Image::fromFile(m_heightfieldSource);

            TextOutput output ("model/heightfield.off");
            output.writeSymbol("OFF\n");
            output.printf("%d %d 0\n", image->width() * image->height(), (image->width() - 1) * (image->height() - 1));

            for(int x = 0; x < image->width(); ++x){
                for(int z = 0; z < image->height(); ++z){
                    Color3 color;
                    image->get(Point2int32(x, z), color);
                    float y = color.average();
                    output.printf("%f %f %f\n", ((float)x)*m_heightfieldXZScale, y*m_heightfieldYScale, ((float)z)*m_heightfieldXZScale);
                }
            }

            for(int i = 1; i < image->height(); ++i){
                for(int j = 1; j < image->width(); ++j){
                    output.printf("4 %d %d %d %d\n", i + ((image->height())*j), i + ((image->height())*j) - 1, i + ((image->height())*(j-1)) - 1, i + ((image->height())*(j-1)));
                }
            }

            output.commit(true);
            G3D::ArticulatedModel::clearCache();
        } catch (...) {
            msgBox("Unable to load the image.", m_heightfieldSource);
        }
    });
}

//Creates a GUI that allows a user to generate a glass object with a given number of rotational slices
void App::makeGlass() {
      GuiPane* glassPane = debugPane->addPane("Glass");

    glassPane->setNewChildSize(240);
        
    glassPane->addNumberBox("Slices", &m_glassSlices, "", 
        GuiTheme::LOG_SLIDER, 3, 100)->setUnitsSize(30);
    
    glassPane->addButton("Generate", [this](){
        drawMessage("Generating Glass.");
        TextOutput output ("model/glass.off");

        float heights [7] = {9, 8, 7.5, 6, 5, 1, 0};
        float radii [7] = {2.5, 3, 2.75, 2, 0.5, 0.5, 2};

        int verticies = m_glassSlices;

        output.writeSymbol("OFF\n");
        output.printf("%d %d 0\n", (verticies*7) + 1, verticies*11);

        for(int j = 0; j < 7; ++j) {
            for(float i = 0; i < verticies; ++i) {
                float a = ((float) 2.0) * pif()*i/(((float)verticies));
                float z = cos(a) * radii[j];
                float x = sin(a) * radii[j];
                output.printf("%f %f %f\n", x, heights[j], z);
            }
         }
        output.printf("0 5 0\n");

         for(int i = 0; i < verticies*6; ++i) {
            output.printf("4 %d %f %f %f\n", i, ((floor(i / verticies) + 1) * verticies) + (i % verticies), ((i + 1) % verticies) + ((floor(i / verticies) + 1) * verticies), ((i+1)%verticies) + (floor(i / verticies) * verticies));
         }

         for(int i = 0; i < verticies*4; ++i) {
            output.printf("4 %d %f %f %f\n", i, ((i+1)%verticies) + (floor(i / verticies) * verticies), ((i + 1) % verticies) + ((floor(i / verticies) + 1) * verticies), ((floor(i / verticies) + 1) * verticies) + (i % verticies));
         }

         for(int i = 0; i < verticies; ++i) {
            output.printf("3 %d %f %f %f\n", i, ((i+1)%verticies) + (floor(i / verticies) * verticies), (verticies*7) + 1);
         }

        output.commit(true);
        G3D::ArticulatedModel::clearCache();
    });
}

App::App(const GApp::Settings& settings) : GApp(settings) {
}

// Called before the application loop begins.  Load data here and
// not in the constructor so that common exceptions will be
// automatically caught.
void App::onInit() {
    debugPrintf("Target frame rate = %f Hz\n", realTimeTargetDuration());
    GApp::onInit();
    setFrameDuration(1.0f / 120.0f);

    // Call setScene(shared_ptr<Scene>()) or setScene(MyScene::create()) to replace
    // the default scene here.
    
    showRenderingStats      = false;

    makeGUI();
    createBall();

    // For higher-quality screenshots:
    // developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    // developerWindow->videoRecordDialog->setCaptureGui(false);
    developerWindow->cameraControlWindow->moveTo(Point2(developerWindow->cameraControlWindow->rect().x0(), 0));
    loadScene(
        //"G3D Sponza"
        "G3D Cornell Box" // Load something simple
        //developerWindow->sceneEditorWindow->selectedSceneName()  // Load the first scene encountered 
        );
}

void App::makeGUI() {
    // Initialize the developer HUD
    createDeveloperHUD();

    debugWindow->setVisible(true);
    developerWindow->videoRecordDialog->setEnabled(true);

    GuiPane* infoPane = debugPane->addPane("Info", GuiTheme::ORNATE_PANE_STYLE);
    
    // Example of how to add debugging controls
    infoPane->addLabel("You can add GUI controls");
    infoPane->addLabel("in App::onInit().");
    infoPane->addButton("Exit", [this]() { m_endProgram = true; });
    infoPane->pack();

    makeCylinder();
    makeHeightfield();
    makeGlass();
    // More examples of debugging GUI controls:
    // debugPane->addCheckBox("Use explicit checking", &explicitCheck);
    // debugPane->addTextBox("Name", &myName);
    // debugPane->addNumberBox("height", &height, "m", GuiTheme::LINEAR_SLIDER, 1.0f, 2.5f);
    // button = debugPane->addButton("Run Simulator");
    // debugPane->addButton("Generate Heightfield", [this](){ generateHeightfield(); });
    // debugPane->addButton("Generate Heightfield", [this](){ makeHeightfield(imageName, scale, "model/heightfield.off"); });

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


// This default implementation is a direct copy of GApp::onGraphics3D to make it easy
// for you to modify. If you aren't changing the hardware rendering strategy, you can
// delete this override entirely.
void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) {
    if (!scene()) {
        if ((submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) && (!rd->swapBuffersAutomatically())) {
            swapBuffers();
        }
        rd->clear();
        rd->pushState(); {
            rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
            drawDebugShapes();
        } rd->popState();
        return;
    }

    GBuffer::Specification gbufferSpec = m_gbufferSpecification;
    extendGBufferSpecification(gbufferSpec);
    m_gbuffer->setSpecification(gbufferSpec);
    m_gbuffer->resize(m_framebuffer->width(), m_framebuffer->height());
    m_gbuffer->prepare(rd, activeCamera(), 0, -(float)previousSimTimeStep(), m_settings.hdrFramebuffer.depthGuardBandThickness, m_settings.hdrFramebuffer.colorGuardBandThickness);

    m_renderer->render(rd, m_framebuffer, scene()->lightingEnvironment().ambientOcclusionSettings.enabled ? m_depthPeelFramebuffer : shared_ptr<Framebuffer>(),
        scene()->lightingEnvironment(), m_gbuffer, allSurfaces);

    // Debug visualizations and post-process effects
    rd->pushState(m_framebuffer); {
        // Call to make the App show the output of debugDraw(...)
        rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
        drawDebugShapes();
        const shared_ptr<Entity>& selectedEntity = (notNull(developerWindow) && notNull(developerWindow->sceneEditorWindow)) ? developerWindow->sceneEditorWindow->selectedEntity() : shared_ptr<Entity>();
        scene()->visualize(rd, selectedEntity, allSurfaces, sceneVisualizationSettings(), activeCamera());

        // Post-process special effects
        m_depthOfField->apply(rd, m_framebuffer->texture(0), m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(), m_settings.hdrFramebuffer.depthGuardBandThickness - m_settings.hdrFramebuffer.colorGuardBandThickness);

        m_motionBlur->apply(rd, m_framebuffer->texture(0), m_gbuffer->texture(GBuffer::Field::SS_EXPRESSIVE_MOTION),
            m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(),
            m_settings.hdrFramebuffer.depthGuardBandThickness - m_settings.hdrFramebuffer.colorGuardBandThickness);
    } rd->popState();

    // We're about to render to the actual back buffer, so swap the buffers now.
    // This call also allows the screenshot and video recording to capture the
    // previous frame just before it is displayed.
    if (submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) {
        swapBuffers();
    }

    // Clear the entire screen (needed even though we'll render over it, since
    // AFR uses clear() to detect that the buffer is not re-used.)
    rd->clear();

    // Perform gamma correction, bloom, and SSAA, and write to the native window frame buffer
    m_film->exposeAndRender(rd, activeCamera()->filmSettings(), m_framebuffer->texture(0), settings().hdrFramebuffer.colorGuardBandThickness.x + settings().hdrFramebuffer.depthGuardBandThickness.x, settings().hdrFramebuffer.depthGuardBandThickness.x);
}


void App::onAI() {
    GApp::onAI();
    // Add non-simulation game logic and AI code here
}


void App::onNetwork() {
    GApp::onNetwork();
    // Poll net messages here
}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    GApp::onSimulation(rdt, sdt, idt);

    // Example GUI dynamic layout code.  Resize the debugWindow to fill
    // the screen horizontally.
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


bool App::onEvent(const GEvent& event) {
    // Handle super-class events
    if (GApp::onEvent(event)) { return true; }

    // If you need to track individual UI events, manage them here.
    // Return true if you want to prevent other parts of the system
    // from observing this specific event.
    //
    // For example,
    // if ((event.type == GEventType::GUI_ACTION) && (event.gui.control == m_button)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == GKey::TAB)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == 'p')) { ... return true; }

    return false;
}


void App::onUserInput(UserInput* ui) {
    GApp::onUserInput(ui);
    (void)ui;
    // Add key handling here based on the keys currently held or
    // ones that changed in the last frame.
}


void App::onPose(Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) {
    GApp::onPose(surface, surface2D);

    // Append any models to the arrays that you want to later be rendered by onGraphics()
}


void App::onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& posed2D) {
    // Render 2D objects like Widgets.  These do not receive tone mapping or gamma correction.
    Surface2D::sortAndRender(rd, posed2D);
}


void App::onCleanup() {
    // Called after the application loop ends.  Place a majority of cleanup code
    // here instead of in the constructor so that exceptions can be caught.
}
