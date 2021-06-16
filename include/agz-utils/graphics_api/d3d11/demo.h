#pragma once

#include <iostream>
#include <memory>

#include <agz-utils/graphics_api/d3d11/window.h>

AGZ_D3D11_BEGIN

class Demo : public misc::uncopyable_t
{
    WindowDesc windowDesc_;

protected:

    std::unique_ptr<Window> window_;

    Keyboard *keyboard_;
    Mouse    *mouse_;

    virtual void initialize() { }

    virtual void frame() { }

    virtual void destroy() { }

public:

    explicit Demo(WindowDesc windowDesc)
        : windowDesc_(std::move(windowDesc)),
          keyboard_(nullptr), mouse_(nullptr)
    {
        
    }

    virtual ~Demo() = default;

    void run()
    {
        try
        {
            window_ = std::make_unique<Window>(windowDesc_);

            keyboard_ = window_->getKeyboard();
            mouse_ = window_->getMouse();

            initialize();
            
            while(!window_->getCloseFlag())
            {
                window_->doEvents();
                window_->newImGuiFrame();

                frame();

                window_->useDefaultRTVAndDSV();
                window_->useDefaultViewport();

                window_->renderImGui();
                window_->swapBuffers();
            }

            destroy();
        }
        catch(const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};

AGZ_D3D11_END
