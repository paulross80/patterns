#include <iostream>

using std::cout;
using std::endl;

#include <vector>
#include <memory>
#include <string>
#include <functional>


// https://refactoring.guru/design-patterns/factory-method
// https://refactoring.guru/design-patterns/visitor


// Simple rectangle
// x, y, width, height
template <typename Type>
struct Rect
{
    Type x{}, y{}, w{}, h{};
};

// 2D Vector (x, y coords)
struct Vec2
{
    float x{}, y{};
};


// Event struct from the SDL library
// https://www.libsdl.org
struct SDL_Event
{
    // This is empty, so the library is not required to compile this file
};


// My GUI rendering class
// (Separation of Concerns)
// This requires forward declarations
class Checkbox;
class Button;

struct RenderUI
{
    // Render every widget type (Visitor)
    void render(const Checkbox& widget) const { cout << "- Rendering a Checkbox" << endl; }
    void render(const Button& widget)   const { cout << "- Rendering a Button"   << endl; }
};

// App theme handles skins, fonts, colors, etc
class AppTheme
{
};



// Widget types enumeration
enum class e_widgetType
{
    PANEL,
    TAB,
    CHECKBOX,
    SLIDER,
    TEXTBOX,
    IMAGE_VIEW,
    BUTTON,
    RADIO_BUTTON,
    RADIO_BUTTON_GROUP
};


// -----------------
// Widget base class
// -----------------
class IWidget
{

    public:

        using t_widgetPtr = std::unique_ptr<IWidget>;


    public:

        // ctor
        IWidget(e_widgetType type, Rect<float>&& dimension);

        // dtor
        virtual ~IWidget() = default;


        // Virtual functions:
        virtual void onClick(const SDL_Event& event) {};
        virtual void onDoubleClick(const SDL_Event& event) {};
        virtual void onRelease(const SDL_Event& event) {};

        virtual void onMouseMotion(const SDL_Event& event) {};
        virtual void onMouseScroll(const SDL_Event& event) {};

        virtual void onMouseOver()  {};
        virtual void onMouseLeave() {};

        // Accept a rendering visitor
        virtual void accept(const RenderUI& renderer) const = 0;


        // Getters:
        auto getWidgetType() const noexcept -> e_widgetType;
        auto getDimension() const noexcept -> const Rect<float>&;

        // Interaction:
        void setVisibility(bool visible);
        void shouldBeResized(bool resizable);
        void shouldBeMoved(bool movable);

        auto isActive() const noexcept -> bool;
        auto isVisible() const noexcept -> bool;
        auto canBeResized() const noexcept -> bool;
        auto canBeMoved() const noexcept -> bool;

        // Move to another position
        void moveTo(const Vec2& position);
        // Apply movement (offset)
        void moveOffset(const Vec2& offset);


    protected:

        // Widget type
        e_widgetType m_type;

        // Position and size
        Rect<float> m_dimension;

        // Other attributes
        bool m_active;
        bool m_visible;
        bool m_resizable;
        bool m_movable;

};


// cpp
// ctor
IWidget::IWidget(e_widgetType type, Rect<float>&& dimension)
    : m_type(type),
      m_dimension(std::move(dimension)),

      // Default settings
      m_active(true),
      m_visible(true),
      m_resizable(false),
      m_movable(false)
{
}



// -------------------
// Example GUI widgets
// -------------------
// Clickable button
class Button final : public IWidget
{

    public:

        // The slot will be called on button click events
        using t_slot = std::function<void()>;

        // Define button size
        enum class e_buttonSize { SMALL, MEDIUM, LARGE };


    public:

        // ctor
        Button(const AppTheme& theme,
               Rect<float>&& dimension, std::string_view text, t_slot&& clickedSlot,
               e_buttonSize buttonSize = e_buttonSize::SMALL);


        // Virtual functions override:
        void onClick(const SDL_Event& event) override;

        // Mouse over/leave modify the hover state
        void onMouseOver()  override;
        void onMouseLeave() override;

        // Accept a rendering visitor
        void accept(const RenderUI& renderer) const override;

        // Getters:
        auto getText() const noexcept -> std::string_view;
        auto getButtonSize() const noexcept -> e_buttonSize;
        auto isOnHover() const -> bool;


    private:

        // Button text
        std::string m_text;
        // Button size
        e_buttonSize m_size;

        // Hover status
        bool m_hover;

        // Callback for button clicked event
        t_slot m_slot;

};


// cpp
// ctor
Button::Button([[maybe_unused]] const AppTheme& theme,
               Rect<float>&& dimension, std::string_view text, t_slot&& clickedSlot, e_buttonSize buttonSize)

    : IWidget(e_widgetType::BUTTON, std::move(dimension)),

      m_text(text),
      m_size(buttonSize),
      m_hover(false),

      m_slot(std::move(clickedSlot))
{
}


// Virtual functions override:
void Button::onClick(const SDL_Event& event)
{

    cout << " -> Button::onClick() -> \'" << m_text << '\'' << endl;

    // If left mouse button is clicked, invoke slot function
    //if (event.button.button == SDL_BUTTON_LEFT)
        std::invoke(m_slot);
}

// Mouse over/leave modify the hover state
void Button::onMouseOver()
{
    m_hover = true;
}

void Button::onMouseLeave()
{
    m_hover = false;
}


// Accept a rendering visitor
void Button::accept(const RenderUI& renderer) const
{
    renderer.render(*this);
}


// Getters:
auto Button::getText() const noexcept -> std::string_view
{
    return m_text;
}

auto Button::getButtonSize() const noexcept -> e_buttonSize
{
    return m_size;
}

auto Button::isOnHover() const -> bool
{
    return m_hover;
}


// A box that can be marked as checked
class Checkbox final : public IWidget
{

    public:

        // ctor
        Checkbox(const AppTheme& theme,
                 Rect<float>&& dimension, std::string text,
                 bool checked = false, bool grayedOut = false);


        // Virtual functions override:
        void onClick(const SDL_Event& event) override;

        // Accept a rendering visitor
        void accept(const RenderUI& renderer) const override;

        // Getters
        auto getText() const noexcept -> std::string_view;
        auto isChecked() const noexcept -> bool;
        auto isGreyedOut() const noexcept -> bool;


    private:

        // Optional text string
        std::string m_text;

        // Current status
        bool m_checked;
        // Can it be checked?
        // or is it grayed out?
        bool m_greyedOut;

};


// cpp
// ctor
Checkbox::Checkbox([[maybe_unused]] const AppTheme& theme,
                   Rect<float>&& dimension, std::string text, bool checked, bool grayedOut)

    : IWidget(e_widgetType::CHECKBOX, std::move(dimension)),

      m_text(std::move(text)),
      m_checked(checked),
      m_greyedOut(grayedOut)
{
}


// Virtual functions override:
void Checkbox::onClick([[maybe_unused]] const SDL_Event& event)
{
    cout << " -> Checkbox::onClick() -> \'" << m_text << '\'' << endl;

    // If it's not disabled, toggle checked status
    if (!m_greyedOut /* && event.button.button == SDL_BUTTON_LEFT */)
        m_checked = !m_checked;
}

// Accept a rendering visitor
void Checkbox::accept(const RenderUI& renderer) const
{
    renderer.render(*this);
}


// Getters
auto Checkbox::getText() const noexcept -> std::string_view
{
    return m_text;
}

auto Checkbox::isChecked() const noexcept -> bool
{
    return m_checked;
}

auto Checkbox::isGreyedOut() const noexcept -> bool
{
    return m_greyedOut;
}



// -------------------------------------------------
// Graphical User Interface main class
// - Widget factory
// - Owns, updates and renders every active widget
// -------------------------------------------------
class UserInterface
{
    public:

        // ctor
        UserInterface(const RenderUI& renderer, const AppTheme& theme)
            : m_theme(theme), m_renderer(renderer)
        {
        }


        // Adds a new GUI widget
        template <typename WidgetType, typename... Args>
        auto add(Args... args) -> IWidget*;

        // Process a system event
        void processEvent(const SDL_Event& event);

        // Renders everything
        void render() const noexcept;


    private:


        // --- Widgets -----------------------------------------------------------
        // Define widgets container data structure
        using t_widgetsContainer = std::vector<IWidget::t_widgetPtr>;

        // Owner of all GUI widgets
        t_widgetsContainer m_widgets;

        // On which element the mouse is over
        IWidget* m_currentMouseOver = nullptr;

        // --- Graphics ----------------------------------------------------------
        // Skin & Theme
        const AppTheme& m_theme;

        // Widgets renderer
        const RenderUI& m_renderer;

};



// --- Template functions implementation ---
// Adds a new GUI widget
//
// Awesome Variadic Templates article:
// https://eli.thegreenplace.net/2014/variadic-templates-in-c/
template <typename WidgetType, typename... Args>
auto UserInterface::add(Args... args) -> IWidget*
{

    // This WidgetType MUST inherit from IWidget
    static_assert(std::is_base_of_v<IWidget, WidgetType>, "<WidgetType> MUST inherit from <IWidget>!");

    // Create widget
    m_widgets.emplace_back(std::make_unique<WidgetType>(m_theme, std::forward<Args>(args)...));

    // Return a raw pointer to it
    return m_widgets.back().get();

}


// cpp
// Process a system event
void UserInterface::processEvent([[maybe_unused]] const SDL_Event& event)
{
    // Process OS events here...
}

void UserInterface::render() const noexcept
{
    // Iterate through every widget
    for (const auto& widget : m_widgets)
        widget->accept(m_renderer);
}


/*

Main entry point

Output:
- Rendering a Checkbox
- Rendering a Checkbox
- Rendering a Button

*/
int main()
{

    // Create the RenderUI & AppTheme objects
    RenderUI rendering;
    AppTheme theming;

    // Create the GUI main class
    UserInterface ui(rendering, theming);

    // Make some UI elements!
    // Checkboxes
    ui.add<Checkbox>(Rect<float>{ 300.0f, 420.0f, 40.0f, 40.0f }, "Use VSync", true);
    ui.add<Checkbox>(Rect<float>{ 300.0f, 470.0f, 40.0f, 40.0f }, "Emit sound effects", false);
    // Buttons
    ui.add<Button>(Rect<float>{ 550.0f, 720.0f, 150.0f, 50.0f }, "Button1", []()
    {
        cout << "Button 1 CLICKED!" << endl;
    });


    // Lets say we are running on a game loop
    // Process system events
    ui.processEvent({});
    // Render graphics
    ui.render();

    return 0;
}
