#include <iostream>

using std::cout;
using std::endl;

#include <vector>
#include <memory>


// https://refactoring.guru/design-patterns/composite
//
// ----------------------------------
// Base class for Behavior Tree nodes
// ----------------------------------
class IBehaviorTreeNode
{
    public:

        // Node status
        enum class e_status
        {
            SUCCESS,
            FAILURE,
            // Running is returned if the action is asynchronous
            // and it needs more time to complete its operations
//            RUNNING,
            UNKNOWN = -1
        };

        // Node type
        enum class e_nodeType
        {
            // Control nodes
            SEQUENCE,
            FALLBACK,
//            PARALLEL,
//            DECORATOR,
            // Execution nodes
            ACTION,
            CONDITION
        };

        // Raw pointer to node
        using t_nodeRawPtr = IBehaviorTreeNode*;


    public:

        // ctor
        IBehaviorTreeNode(e_nodeType type);

        // dtor
        virtual ~IBehaviorTreeNode() = default;


        // Virtual functions to override:
        virtual auto update(float dt) -> e_status = 0;

        // Getters:
        auto getNodeType() const noexcept -> e_nodeType;
        auto getParent() const noexcept -> t_nodeRawPtr;
        auto hasChildren() const noexcept -> bool;
        auto getChildren() const -> const std::vector<t_nodeRawPtr>&;

        // Add children
        void addChildren(t_nodeRawPtr node);


    protected:

        // Node type
        e_nodeType m_type;

        // Parent node
        t_nodeRawPtr m_parent;
        // Children nodes
        std::vector<t_nodeRawPtr> m_children;

};


// cpp
// ctor
IBehaviorTreeNode::IBehaviorTreeNode(e_nodeType type)
    : m_type(type),
      m_parent(nullptr)
{
}


// Getters:
auto IBehaviorTreeNode::getNodeType() const noexcept -> e_nodeType
{
    return m_type;
}

auto IBehaviorTreeNode::getParent() const noexcept -> t_nodeRawPtr
{
    return m_parent;
}

auto IBehaviorTreeNode::hasChildren() const noexcept -> bool
{
    return !m_children.empty();
}

auto IBehaviorTreeNode::getChildren() const -> const std::vector<t_nodeRawPtr>&
{
    return m_children;
}


// Add children
void IBehaviorTreeNode::addChildren(t_nodeRawPtr node)
{
    m_children.push_back(node);
}


// Utility
auto nodeStatusToString(IBehaviorTreeNode::e_status status) -> std::string_view
{
    switch(status)
    {
        case IBehaviorTreeNode::e_status::FAILURE:
            return "FAILURE";
            break;
        case IBehaviorTreeNode::e_status::SUCCESS:
            return "SUCCESS";
            break;
        case IBehaviorTreeNode::e_status::UNKNOWN:
            return "UNKNOWN!";
            break;
    }

    return {};
}



// ----------------------------
// Example nodes implementation
// ----------------------------

// Control nodes:
class Fallback final : public IBehaviorTreeNode
{
    public:

        // ctor
        Fallback()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::FALLBACK)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Running FALLBACK" << endl;

            // Iterate over all of the children nodes
            for (const auto child : this->m_children)
            {
                // As soon as one child is successful, the entire fallback/strategy is successful
                if (child->update(dt) == IBehaviorTreeNode::e_status::SUCCESS)
                    return IBehaviorTreeNode::e_status::SUCCESS;
            }

            // Return failure!
            return IBehaviorTreeNode::e_status::FAILURE;
        }

};

//
class Sequence final : public IBehaviorTreeNode
{
    public:

        // ctor
        Sequence()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::SEQUENCE)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Running SEQUENCE" << endl;

            // Iterate over all of the children nodes
            for (const auto child : this->m_children)
            {
                // One child node fails, the entire sequence fails too
                if (child->update(dt) == IBehaviorTreeNode::e_status::FAILURE)
                    return IBehaviorTreeNode::e_status::FAILURE;
            }

            // Return success!
            return IBehaviorTreeNode::e_status::SUCCESS;
        }

};


// Execution nodes:
//
class Esto final : public IBehaviorTreeNode
{
    public:

        // ctor
        Esto()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::ACTION)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Esto -> " << nodeStatusToString(IBehaviorTreeNode::e_status::FAILURE) << endl;
            return IBehaviorTreeNode::e_status::FAILURE;
        }

};

//
class Aquello final : public IBehaviorTreeNode
{
    public:

        // ctor
        Aquello()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::ACTION)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Aquello -> " << nodeStatusToString(IBehaviorTreeNode::e_status::FAILURE) << endl;
            return IBehaviorTreeNode::e_status::FAILURE;
        }

};

//
class Uno final : public IBehaviorTreeNode
{
    public:

        // ctor
        Uno()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::ACTION)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Uno -> " << nodeStatusToString(IBehaviorTreeNode::e_status::SUCCESS) << endl;
            return IBehaviorTreeNode::e_status::SUCCESS;
        }

};

//
class Dos final : public IBehaviorTreeNode
{
    public:

        // ctor
        Dos()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::ACTION)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Dos -> " << nodeStatusToString(IBehaviorTreeNode::e_status::SUCCESS) << endl;
            return IBehaviorTreeNode::e_status::SUCCESS;
        }

};

//
class Tres final : public IBehaviorTreeNode
{
    public:

        // ctor
        Tres()
            : IBehaviorTreeNode(IBehaviorTreeNode::e_nodeType::ACTION)
        {}

        // Virtual functions to override:
        auto update(float dt) -> e_status override
        {
            cout << "Tres -> " << nodeStatusToString(IBehaviorTreeNode::e_status::SUCCESS) << endl;
            return IBehaviorTreeNode::e_status::SUCCESS;
        }

};



// -----------------------------
// Main class for behavior trees
// -----------------------------
class BehaviorTree
{
    public:

        // Creates a new node
        template <typename NodeClass, typename... Args>
        auto create(Args... args) -> IBehaviorTreeNode::t_nodeRawPtr;

        // Iterates over the entire tree
        void run(float dt) const;

        // Set the root node
        void setRoot(IBehaviorTreeNode::t_nodeRawPtr node);

        // Get the root node of the tree
        auto getRoot() const noexcept -> IBehaviorTreeNode::t_nodeRawPtr;


    private:

        // Root node
        IBehaviorTreeNode::t_nodeRawPtr m_root;

        // Node storage
        std::vector<std::unique_ptr<IBehaviorTreeNode>> m_nodes;

};


// --- Template functions implementation ---
// Creates a new node
template <typename NodeClass, typename... Args>
auto BehaviorTree::create(Args... args) -> IBehaviorTreeNode::t_nodeRawPtr
{

    // Must be a valid derived class
    static_assert(std::is_base_of_v<IBehaviorTreeNode, NodeClass>,
        "[C++] BehaviorTree::create(): <NodeClass> class must be derived from <IBehaviorTreeNode> class!");

    // Make a new object in place and return a reference to it (std::unique_ptr<NodeClass>)
    auto& reference = m_nodes.emplace_back(std::make_unique<NodeClass>(std::forward<Args>(args)...));

    // Return a reference to
    // the underlying raw pointer
    return reference.get();

}


// cpp
// Set the root node
void BehaviorTree::setRoot(IBehaviorTreeNode::t_nodeRawPtr node)
{
    m_root = node;
}

//
void BehaviorTree::run(float dt) const
{
    m_root->update(dt);
}


// Get the root node of the tree
auto BehaviorTree::getRoot() const noexcept -> IBehaviorTreeNode::t_nodeRawPtr
{
    return m_root;
}



/*

Main entry point

Output:
Running FALLBACK
Esto -> FAILURE
Aquello -> FAILURE
Running SEQUENCE
Uno -> SUCCESS
Dos -> SUCCESS
Tres -> SUCCESS

*/
int main()
{

    // Create a behavior tree
    BehaviorTree bt;

    // Create some nodes
    auto* sequence1 = bt.create<Sequence>();
    auto* fallback1 = bt.create<Fallback>();
    auto* esto = bt.create<Esto>();
    auto* aquello = bt.create<Aquello>();
    auto* uno = bt.create<Uno>();
    auto* dos = bt.create<Dos>();
    auto* tres = bt.create<Tres>();

    // Add nodes to fallback 1
    fallback1->addChildren(esto);
    fallback1->addChildren(aquello);
    fallback1->addChildren(sequence1);

    // Add nodes to sequence 1
    sequence1->addChildren(uno);
    sequence1->addChildren(dos);
    sequence1->addChildren(tres);

    // The root node will be the first fallback in this case
    bt.setRoot(fallback1);

    // Run from the root node
    bt.run(1.0f / 60.0f);

    return 0;
}
