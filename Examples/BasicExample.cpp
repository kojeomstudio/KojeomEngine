#include "../Engine/Core/Engine.h"

/**
 * @brief Basic engine example application
 * 
 * Simple example using the new KojeomEngine structure.
 * Renders a blue background and displays FPS.
 */
class BasicExample : public KEngine
{
public:
    BasicExample() = default;
    ~BasicExample() = default;

    /**
     * @brief Update logic (called every frame)
     */
    void Update(float deltaTime) override
    {
        // Call parent class's default update
        KEngine::Update(deltaTime);

        // Add game-specific update logic here
        // Example: input processing, game object updates, etc.
    }

    /**
     * @brief Rendering logic (called every frame)
     */
    void Render() override
    {
        if (!GetGraphicsDevice())
            return;

        // Begin frame - clear background
        float clearColor[4] = { 0.0f, 0.3f, 0.6f, 1.0f }; // Blue background
        GetGraphicsDevice()->BeginFrame(clearColor);

        // Add actual rendering logic here
        // Example: 3D models, UI rendering, etc.

        // End frame - present back buffer
        GetGraphicsDevice()->EndFrame(true);
    }
};

/**
 * @brief Application entry point
 */
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Use improved engine helper function
    return KEngine::RunApplication<BasicExample>(
        hInstance,
        L"KojeomEngine - Basic Example",
        1024, 768
    );
} 