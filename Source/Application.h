#pragma once

namespace Mango
{

    class MangoApplication
    {
    public:
        void Run();

    private:
        void InitializeVulkan();
        void RunMainLoop();
        void Dispose();
    };

}
