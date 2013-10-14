#include "World.hpp"
#include "Planet.hpp"
#include "Ship.hpp"

#include <Box2D/Box2D.h>
#include <random>

#ifdef DEBUG
#include "../Debug/DebugDraw.hpp"
#endif

using namespace Game;

World::World() : mBox2DWorld(nullptr)
#ifdef DEBUG
    , mDebugDraw(nullptr)
#endif
{

}

World::~World()
{
    mPlanets.clear();
    mShips.clear();

    if (mBox2DWorld)
        delete mBox2DWorld;
    if (mDebugDraw)
        delete mDebugDraw;
}

void World::init()
{
    if (mBox2DWorld)
        return;

    mBox2DWorld = new b2World(b2Vec2(0,0));
}

void World::update(float dt)
{
    if (!mBox2DWorld)
        return;

    static int velocitySteps = 8;
    static int positionSteps = 4;

    mBox2DWorld->Step(dt, velocitySteps, positionSteps); ///\TODO Maybe check if this needs to be more or less static anywho

    FOR_EACH (auto& p, mPlanets)
    {
        if (p.getDirty())
        {
            p.resetDirty();

            ///\TODO Calculate if planet needs to be replaced.
        }
    }

    auto dist = [](const sf::Vector2f& a, const sf::Vector2f& b) -> float {
        return ((b.x-a.x)*(b.x-a.x))+((b.y-a.y)*(b.y-a.y));
    };

    FOR_EACH (auto& s, mShips)
    {
        FOR_EACH (auto& p, mPlanets)
        {
            if (dist(s.getPosition(), p.getPosition()) < 256*256)
                s.addGravity(p.getPosition());
        }
    }
}

///\FIXME Don't draw everything always.
void World::draw(sf::RenderTarget& target)
{
#ifdef DEBUG
    if (!mDebugDraw)
    {
        uint32 flags = (b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_shapeBit);

        mDebugDraw = new DebugDraw(target);
        mDebugDraw->SetFlags(flags);
        mBox2DWorld->SetDebugDraw(mDebugDraw);
    }

    mBox2DWorld->DrawDebugData();
#else
    FOR_EACH (auto& p, mPlanets)
    {
        p.draw(target);
    }

    FOR_EACH(auto& s, mShips)
    {
        s.draw(target);
    }
#endif
}

void World::addPlanet(const Planet& p)
{
    Game::Planet tmp = p;

    std::random_device dev;
    std::uniform_real_distribution<float> distX(0, mSize.x);
    std::uniform_real_distribution<float> distY(0, mSize.y);

    tmp.setPosition(sf::Vector2f(distX(dev) - mSize.x / 2.f, distY(dev) - mSize.y / 2.f));
    tmp.addedToWorld(*this);

    mPlanets.push_back(tmp);
}

void World::addShip(const Ship& s)
{
    Game::Ship tmp = s;

    std::random_device dev;
    std::uniform_real_distribution<float> distX(0, mSize.x);
    std::uniform_real_distribution<float> distY(0, mSize.y);

    //tmp.setPosition(sf::Vector2f(distX(dev) - mSize.x / 2.f, distY(dev) - mSize.y / 2.f));
    tmp.addedToWorld(*this);

    mShips.push_back(tmp);
}