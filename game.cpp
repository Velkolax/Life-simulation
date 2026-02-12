#include "game.h"
#include "game_configdata.h"
#include <set>
#include <ft2build.h>
#include "BacteriaData.h"
#include  <random>
#include FT_FREETYPE_H
#include "resource_manager.h"
#include "simulation_engine.h"

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char* message, const void* userParam) {
    if (severity==GL_DEBUG_SEVERITY_MEDIUM)
    {
        std::cout << "OpenGL Warning Message: " << message << std::endl;
    }
    else std::cerr << "OpenGL Error Message: " << message << std::endl;
}

BacteriaWidget::BacteriaWidget(QWidget* parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&BacteriaWidget::update));
    timer->start(16);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
}

BacteriaWidget::~BacteriaWidget()
{
    delete Renderer;
    delete engine;
    delete Text;
    ResourceManager::Clear();
}

void BacteriaWidget::initializeGL()
{
    initializeOpenGLFunctions();
    this->Width = width();
    this->Height = height();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);

    ResourceManager::LoadShader({"shaders/instance.vs.glsl"},{"shaders/instance.fs.glsl"},"instance");
    ResourceManager::LoadShader({"shaders/instance.vs.glsl"},{"shaders/instance_bac.fs.glsl"},"instance_bac");
    ResourceManager::LoadComputeShader({"shaders/process_network.cs.glsl"},"network");
    ResourceManager::LoadComputeShader({"shaders/init_weights.cs.glsl"},"init");
    ResourceManager::LoadComputeShader({"shaders/kill.cs.glsl"},"kill");
    ResourceManager::LoadComputeShader({"shaders/reproduce.cs.glsl"},"reproduce");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/energy.png",true,"apple");
    ResourceManager::LoadTexture("textures/explosion.png",true,"explosion");
    ResourceManager::LoadTexture("textures/acid.png",true,"acid");
    ResourceManager::LoadTexture("textures/protein.png",true,"protein");


    substeps = GameConfigData::getInt("substeps");
    int seed = GameConfigData::getInt("seed");
    std::cout << "Seed: " << seed << std::endl;
    if (!seed)
    {
        std::mt19937 g{std::random_device{}()};
        std::cout << "AAA" << std::endl;
        GameConfigData::setInt("seed",std::to_string(std::uniform_int_distribution<int>{0,100000}(g)));
    }
    std::cout << "Seed: " << GameConfigData::getInt("seed") << std::endl;
    gen.seed(GameConfigData::getInt("seed"));
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = GameConfigData::getInt("bacteriaCount");
    int x = GameConfigData::getInt("width");
    int y = GameConfigData::getInt("height");
    board = new Board(x, y, this,bacteriaCount);
    board->InitializeNeighbour((x-2)/2, true);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("instance"),board,Width,Height,this);

    board->spawnBacteria();
    board->spawnFood(0.1);
    engine = new SimulationEngine(board);

}

void BacteriaWidget::paintGL()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->tick();
    Renderer -> DrawBoard(board,Width,Height);


    //Text->RenderText("NUMBER OF BACTERIA: "+std::to_string(board->getAliveBacteriaCount()),Width*0.5,10,1.0);
    emit statsUpdated(board->getAliveBacteriaCount());
    //Text->RenderText("STEP: "+std::to_string(board->getStep()),Width*0.5,40,1.0);
    emit stepInfoUpdated(board->getStep());
    //Text->RenderText("SPECIES LEFT: "+std::to_string(board->getAliveSpeciesNumber()),Width*0.5,70,1.0);
    emit speciesInfoUpdated(board->getAliveSpeciesNumber());
}

void BacteriaWidget::resizeGL(int w, int h)
{
    this->Width = w;
    this->Height = h;
    glViewport(0, 0, w, h);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("instance")->Use().SetMatrix4("projection", projection);
    //ResourceManager::GetShader("instance_bac")->Use().SetMatrix4("projection", projection);
    if (Text) {
        Text->TextShader->Use().SetMatrix4("projection",
            glm::ortho(0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f));
    }
}

void BacteriaWidget::keyPressEvent(QKeyEvent* event)
{
    input.setKeyState(event->key(),true);
}

void BacteriaWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (!event->isAutoRepeat())
        input.setKeyState(event->key(),false);
}

void BacteriaWidget::mousePressEvent(QMouseEvent* event)
{
    input.setButtonState(event->button(), true);
}

void BacteriaWidget::mouseReleaseEvent(QMouseEvent* event)
{
    input.setButtonState(event->button(), false);
}

void BacteriaWidget::mouseMoveEvent(QMouseEvent* event)
{
    input.cursorX=event->pos().x();
    input.cursorY=event->pos().y();
}

void BacteriaWidget::wheelEvent(QWheelEvent* event)
{
    input.scrollDelta = event->angleDelta().y();
}

void BacteriaWidget::tick()
{
    if (input.isDown(Qt::Key_W)) Renderer -> addToDisplacementY(board,10);
    if (input.isDown(Qt::Key_A)) Renderer ->addToDisplacementX(board,10);
    if (input.isDown(Qt::Key_S)) Renderer -> addToDisplacementY(board,-10);
    if (input.isDown(Qt::Key_D)) Renderer -> addToDisplacementX(board,-10);

    if (input.isButtonDown(Qt::LeftButton)) {
        float size = Renderer -> getSize(board);
        glm::ivec2 p = Renderer -> CheckWhichHexagon(input.cursorX,input.cursorY,size/2);
        Hexagon *hex = board->getHexagon(p.y*board->getWidth()+p.x);
        if (hex!=nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = board->getBacteria(res.bacteriaIndex);
            Text->RenderText("AGE: "+ std::to_string(bac.age),10,10,1.0);
            Text->RenderText("ACID: "+ std::to_string(bac.acid),10,40,1.0);
            Text->RenderText("ENERGY: "+ std::to_string(bac.energy),10,70,1.0);
            Text->RenderText("PROTEIN: "+std::to_string(bac.protein),10,100,1.0);
            Text->RenderText("SPEED: " + std::to_string(bac.speed),10,130,1.0);
            Text->RenderText("LIFESPAN: " + std::to_string(bac.lifespan),10,160,1.0);
            Text->RenderText("LAST ACTION: "+stringActions[(int)bac.lastAction],10,190,1.0);
            Text->RenderText("MOTHRED: "+std::to_string(bac.mothered),10,220,1.0);
            Text->RenderText("FATHERED: "+std::to_string(bac.fathered),10,250,1.0);
            Text->RenderText("SPECIES: "+std::to_string(hex->getClan()),10,280,1.0);
            Text->RenderText("POPULATION: "+std::to_string(board->getNumberOfMembersOfSpecies(hex->getClan())),10,310,1.0);
        }
        if (hex!=nullptr && protein(hex->getResident()))
        {
            ProteinData res = hex->getData().protein;
            auto a = res.amount;
            Text->RenderText("PROTEIN AMOUNT: "+ std::to_string(a),10,10,1.0 );
        }
        if (hex!=nullptr && energy(hex->getResident()))
        {
            EnergyData res = hex->getData().energy;
            auto a = res.amount;
            Text->RenderText("ENERGY AMOUNT: "+ std::to_string(a),10,10,1.0 );
        }
        if (hex!=nullptr && acid(hex->getResident()))
        {
            AcidData res = hex->getData().acid;
            auto a = res.amount;
            Text->RenderText("ACID AMOUNT: "+ std::to_string(a),10,10,1.0 );
        }
    }

    if (input.scrollDelta != 0) {
        float zoomFactor = (input.scrollDelta > 0) ? 1.1f : 0.9f;
        Renderer->Zoom(zoomFactor, input.cursorX, input.cursorY, board);
    }

    if (input.isReleased(Qt::Key_Return))
        for (int i=0;i<substeps;i++)
            board->tick();
    else if (input.isDown(Qt::Key_Space))
        for (int i=0;i<substeps;i++)
            board->tick();
    else if (input.isToggled(Qt::Key_P))
        for (int i=0;i<substeps;i++)
            board->tick();
    //board->tick();
    input.update();
}

void BacteriaWidget::restart()
{
}
