//----------------------------------------------------------------------------
/** @file SgNode.cpp
    See SgNode.h.

    Implementation details:
    -----------------------
    Each node has a pointer to its father, its right brother, and its first
    son. Each node also has a pointer to its list of properties, as well as an
    extra bit used to efficiently find the shortest path between two nodes.
    This data structure assumes that time and not space is the major
    performance bottleneck.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgNode.h"

#include <sstream>
#include "SgPointSet.h"
#include "SgProp.h"
#include "SgVector.h"

using namespace std;

//----------------------------------------------------------------------------

SgNode::SgNode()
    : m_son(0),
      m_father(0),
      m_brother(0),
      m_props(),
      m_marked(false)
{
#ifndef NDEBUG
    ++s_alloc;
#endif
}

SgNode::~SgNode()
{
    if (HasLeftBrother())
    {
        SgNode* left = LeftBrother();
        //--- Just link left brother with son. If the son is 0, then the same
        //  node will be linked to the brother by procedure LinkToBrother.
        left->m_brother = m_son;
        LinkWithBrother(left);
    }
    else if (HasFather())
    {
        if (HasSon())
        {
            //--- Has son: father links up with that son, must link rightmost
            //      brother of that son to right brother of deleted node.
            m_father->m_son = m_son;
            m_son->m_father = m_father;
            LinkWithBrother(m_son);
        }
        else
        {
            //--- Simple case: is leftmost node and has no sons, father
            //      can just link up with an eventual brother.
            m_father->m_son = m_brother;
        }
    }
    else
        // --> node is root <--
        // cannot delete the root node if it has a subtree
        SG_ASSERT(m_son == 0);

#ifndef NDEBUG
    ++s_free;
#endif
}

void SgNode::CopySubtree(const SgNode* node, SgNode* copy)
{
    for (const SgNode* son = node->LeftMostSon(); son;
         son = son->RightBrother())
    {
        SgNode* sonCopy = copy->NewRightMostSon();
        sonCopy->CopyAllPropsFrom(*son);
        CopySubtree(son, sonCopy);
    }
}

SgNode* SgNode::CopyTree() const
{
    SgNode* newNode = new SgNode();
    if (newNode)
    {
        newNode->CopyAllPropsFrom(*this);
        CopySubtree(this, newNode);
    }
    return newNode;
}

bool SgNode::HasLeftBrother() const
{
    return HasFather() && Father()->LeftMostSon() != this;
}

bool SgNode::IsOnMain() const
{
    // Move backwards in the tree until the root or a node with a
    // left brother is reached.
    const SgNode* t = this;
    while (true)
    {
        SgNode* father = t->m_father;
        if (father == 0)
            return true;
        if (father->m_son != t)
            return false;
        t = father;
    }
}

// Links the rightmost brother of the current node with the brother of this
// node that is to be deleted.
void SgNode::LinkWithBrother(SgNode* node)
{
    while (node->HasRightBrother())
    {
        node = node->RightBrother(); // sons of deleted node ...
        node->m_father = m_father; //      ... get this new father
    }
    node->m_brother = m_brother;
}

SgVector<SgPoint> SgNode::VectorProp(SgPropID prop) const
{
    SgPropPointList* lp = dynamic_cast<SgPropPointList*>(Get(prop));
    if (lp)
        return lp->Value();
    return SgVector<SgPoint>();
}

int SgNode::NumSons() const
{
    int n = 0;
    if (HasSon())
    {
        SgNode* node = LeftMostSon();
        ++n;
        while (node->HasRightBrother())
        {
            node = node->RightBrother();
            ++n;
        }
    }
    return n;
}

int SgNode::NumLeftBrothers() const
{
    int numBrothers = 0;
    SgNode* node = Father();
    if (node)
    {
        node = node->LeftMostSon();
        while (node != this)
        {
            node = node->RightBrother();
            ++numBrothers;
        }
    }
    return numBrothers;
}

SgNode* SgNode::Root() const
{
    SgNode* node = const_cast<SgNode*>(this);
    while (node->HasFather())
        node = node->Father();
    return node;
}

SgNode* SgNode::LeftBrother() const
{
    if (! HasFather() || ! HasLeftBrother())
        return 0;
    SgNode* node = Father()->LeftMostSon(); // left-most brother
    while (node->RightBrother() != this)
        node = node->RightBrother();
    return node;
}

SgNode* SgNode::RightMostSon() const
{
    SgNode* node = LeftMostSon();
    if (node)
    {
        while (node->HasRightBrother())
            node = node->RightBrother();
    }
    return node;
}

SgNode* SgNode::NextDepthFirst() const
{
    if (HasSon())
        return LeftMostSon();
    else
    {
        SgNode* node = const_cast<SgNode*>(this);
        while (node->HasFather() && ! node->HasRightBrother())
            node = node->Father();
        if (node->HasRightBrother())
            node = node->RightBrother();
        return node;
    }
}

SgNode* SgNode::PrevDepthFirst() const
{
    SgNode* node = const_cast<SgNode*>(this);
    if (HasLeftBrother() || ! HasFather())
    {
        if (HasLeftBrother())
            node = LeftBrother();
        while (node->HasSon())
            node = node->RightMostSon();
    }
    else
        node = Father();
    return node;
}

void SgNode::PathToRoot(SgVectorOf<SgNode>* path) const
{
    path->Clear();
    for (SgNode* node = const_cast<SgNode*>(this); node; 
         node = node->Father())
        path->PushBack(node);
}

SgNode* SgNode::NodeInDirection(Direction dir) const
{
    SgNode* node = 0;
    switch (dir)
    {
    case PREVIOUS:
        node = Father();
        break;
    case NEXT:
        node = LeftMostSon();
        break;
    case NEXT_RIGHTMOST:
        node = RightMostSon();
        break;
    case PREV_DEPTHFIRST:
        node = PrevDepthFirst();
        break;
    case NEXT_DEPTHFIRST:
        node = NextDepthFirst();
        break;
    case PREV_TERMINAL:
        node = PrevDepthFirst();
        while (! node->IsTerminal())
            node = node->PrevDepthFirst();
        break;
    case NEXT_TERMINAL:
        node = NextDepthFirst();
        while (! node->IsTerminal())
            node = node->NextDepthFirst();
        break;
    case PREV_BRANCH:
        node = Father();
        while (node && node->HasFather() && ! node->IsBranchPoint())
            node = node->Father();
        break;
    case NEXT_BRANCH:
        node = LeftMostSon();
        while (node && ! node->IsTerminal() && ! node->IsBranchPoint())
            node = node->LeftMostSon();
        break;
    case LEFT_BROTHER:
        if (HasLeftBrother())
            node = LeftBrother();
        else if (HasBrother())
            node = Father()->RightMostSon(); // wrap around
        break;
    case RIGHT_BROTHER:
        if (HasRightBrother())
            node = RightBrother();
        else if (HasBrother())
            node = Father()->LeftMostSon(); // wrap around
        break;
    case MAIN_BRANCH:
        {
            SgNode* t = const_cast<SgNode*>(this);
            while (t->HasFather())
            {
                if (t->HasLeftBrother())
                    node = t->Father();
                t = t->Father();
            }
        }
        break;
    case START_OF_GAME:
        node = Root();
        break;
    case END_OF_GAME:
        node = const_cast<SgNode*>(this);
        while (! node->IsTerminal())
            node = node->LeftMostSon();
        break;
    default:
        SG_ASSERT(false);
    }
    return node;
}

bool SgNode::ContainsText(const string& findText)
{
    // Handle check for special properties outside of this function.
    SG_ASSERT(SgProp::ConvertFindTextToPropID(findText) == SG_PROP_NONE);
    return m_props.GetPropContainingText(findText) != 0;
}

// add comment to SG_PROP_COMMENT of node
void SgNode::AddComment(const std::string& comment)
{
    SgPropText* textProp = static_cast<SgPropText*>(Get(SG_PROP_COMMENT));
    if (textProp)
        textProp->AppendText(comment);
    else
    {
        textProp = new SgPropText(SG_PROP_COMMENT, comment);
        Add(textProp);
    }
}

bool SgNode::HasProp(SgPropID id) const
{
    if (id == SG_PROP_TERMINAL)
        return IsTerminal();
    else if (id == SG_PROP_BRANCH)
        return IsBranchPoint();
    else
        return Get(id) != 0;
}

// shortest path between two nodes
// To do this efficiently, we mark all nodes from the two nodes back toward
// the root until we find a node that's already marked.
void SgNode::ShortestPathTo(SgNode* node, int* numBack,
                            SgVectorOf<SgNode>* path) const
{
    //--- Go backwards from both nodes in parallel, marking all nodes, until
    //     finding a marked node (the common ancestor), or both are 0.
    SgNode* x = const_cast<SgNode*>(this);
    SgNode* y = node;
    SgNode* common = 0;
    while (true)
    {
        if (x)
        {
            if (x->IsMarked())
            {
                common = x;
                break;
            }
            x->Mark();
            x = x->m_father;
        }
        if (y)
        {
            if (y->IsMarked())
            {
                common = y;
                break;
            }
            y->Mark();
            y = y->m_father;
        }
        if (! x && ! y)
        {
            break;
        }
    }

    //--- Unmark all nodes from 'common' toward the root.
    
    for (x = common; x && x->IsMarked(); x = x->m_father)
    {
        x->Unmark();
    }

    //--- Unmark and count all nodes from this node to 'common'.
    *numBack = 0;
    
    for (x = const_cast<SgNode*>(this); x != common; x = x->m_father)
    {
        SG_ASSERT(x->IsMarked());
        x->Unmark();
        ++(*numBack);
    }

    //--- Unmark and store all nodes from 'node' to 'common'.
    path->Clear();
    for (x = node; x != common; x = x->m_father)
    {
        SG_ASSERT(x->IsMarked());
        x->Unmark();
        path->PushBack(x);
    }
    path->Reverse();
}

void SgNode::PromoteNode()
{
    if (HasLeftBrother())
    {
        SgNode* brotherOfThis = m_brother;
        m_brother = m_father->m_son; // first son is brother
        m_father->m_son = this; // father has new first son
        SgNode* x = this;
        while (x->m_brother != this)
            x = x->m_brother;
        x->m_brother = brotherOfThis; // skip moved node
    }
}

void SgNode::PromotePath()
{
    SgNode* node = this;
    while (node)
    {
        node->PromoteNode(); // promote all nodes on the path to the root
        node = node->Father();
    }
}

void SgNode::DeleteSubtree()
{
    // 'delete' does all the work of keeping the tree pointers consistent, so
    // can just keep deleting until no nodes below the current node remain.
    while (HasSon())
        delete LeftMostSon();
}

void SgNode::DeleteBranches()
{
    SgNode* main = LeftMostSon();
    while (main)
    {
        SgNode* brother = main->RightBrother();
        while (brother)
        {
            SgNode* nextToDelete = brother->RightBrother();
            brother->DeleteSubtree();
            delete brother;
            brother = nextToDelete;
        }
        main = main->LeftMostSon();
    }
}

void SgNode::DeleteTree()
{
    SgNode* root = Root();
    root->DeleteSubtree();
    delete root;
}

SgNode* SgNode::NewFather()
{
    SgNode* n = new SgNode();
    if (HasLeftBrother())
    {
        SgNode* left = LeftBrother();
        left->m_brother = n;
    }
    else
        m_father->m_son = n;
    n->m_son      = this;
    n->m_brother = m_brother;
    n->m_father  = m_father;
    m_brother = 0;
    m_father = n;
    return n;
}

SgNode* SgNode::NewRightBrother()
{
    SgNode* n = new SgNode();
    n->m_brother = m_brother;
    n->m_father  = m_father;
    m_brother = n;
    return n;
}

SgNode* SgNode::NewLeftMostSon()
{
    SgNode* n = new SgNode();
    n->m_father = this;
    n->m_brother = m_son;
    m_son = n;
    return n;
}

SgNode* SgNode::NewRightMostSon()
{
    if (HasSon())
        return RightMostSon()->NewRightBrother();
    else
        return NewLeftMostSon();
}

SgNode* SgNode::LinkTrees(const SgVectorOf<SgNode>& roots)
{
    SgNode* super = new SgNode();
    SgNode* previous = 0;
    for (SgVectorIteratorOf<SgNode> iter(roots); iter; ++iter)
    {
        SgNode* root = *iter;
        SG_ASSERT(! root->HasFather());
        if (previous)
            previous->m_brother = root;
        else
            super->m_son = root;
        root->m_father = super;
        previous = root;
    }
    return super;
}

void SgNode::AppendTo(SgNode* n)
{
    SG_ASSERT(! HasFather());
    SG_ASSERT(! HasBrother());
    SG_ASSERT(n);
    if (n->HasSon())
    {
        SgNode* t = n->RightMostSon();
        t->m_brother = this;
    }
    else
        n->m_son = this;
    m_father = n;
}

SgNode* SgNode::TopProp(SgPropID id) const
{
    SgNode* node = const_cast<SgNode*>(this);
    while (node->HasFather() && ! node->Get(id))
        node = node->Father();
    // Return either root node or node with property.
    return node;
}

int SgNode::GetIntProp(SgPropID id) const
{
    SgPropInt* prop = static_cast<SgPropInt*>(Get(id));
    if (prop)
        return prop->Value();
    else
        return 0;
}

bool SgNode::GetIntProp(SgPropID id, int* value) const
{
    SgPropInt* prop = static_cast<SgPropInt*>(Get(id));
    if (prop)
    {   *value = prop->Value();
        return true;
    }

    return false;
}

bool SgNode::HasNodeMove() const
{
    return HasProp(SG_PROP_MOVE_BLACK) || HasProp(SG_PROP_MOVE_WHITE);
}

SgBlackWhite SgNode::NodePlayer() const
{
    SG_ASSERT(HasNodeMove());
    if (HasProp(SG_PROP_MOVE_BLACK))
        return SG_BLACK;
    return SG_WHITE;
}

SgPoint SgNode::NodeMove() const
{
    SgPoint p;
    if (GetIntProp(SG_PROP_MOVE_BLACK, &p))
        return p;
    else if (GetIntProp(SG_PROP_MOVE_WHITE, &p))
        return p;
    else
        return SG_NULLMOVE;
}

double SgNode::GetRealProp(SgPropID id) const
{
    SgPropReal* prop = dynamic_cast<SgPropReal*>(Get(id));
    if (prop)
        return prop->Value();
    else
        return 0;
}

void SgNode::SetIntProp(SgPropID id, int value)
{
    SgPropInt* prop = dynamic_cast<SgPropInt*>(Get(id));
    if (prop)
        prop->SetValue(value);
    else
    {
        prop = dynamic_cast<SgPropInt*>(SgProp::CreateProperty(id));
        prop->SetValue(value);
        Add(prop);
    }
}

void SgNode::SetRealProp(SgPropID id, double value, int precision)
{
    SgPropReal* prop = dynamic_cast<SgPropReal*>(Get(id));
    if (prop)
        prop->SetValue(value);
    else
    {
        prop = static_cast<SgPropReal*>(SgProp::CreateProperty(id));
        prop->SetValue(value, precision);
        Add(prop);
    }
}

bool SgNode::GetStringProp(SgPropID id, string* value) const
{
    SgPropText* prop = dynamic_cast<SgPropText*>(Get(id));
    if (prop)
    {
        *value = prop->Value();
        return true;
    }
    return false;
}

void SgNode::SetStringProp(SgPropID id, const string& value)
{
    SgPropText* prop = dynamic_cast<SgPropText*>(Get(id));
    if (prop)
        prop->SetValue(value);
    else
    {
        prop = static_cast<SgPropText*>(SgProp::CreateProperty(id));
        prop->SetValue(value);
        Add(prop);
    }
}

void SgNode::SetListProp(SgPropID id, const SgVector<SgPoint>& value)
{
    SgPropPointList* prop = dynamic_cast<SgPropPointList*>(Get(id));
    if (prop)
        prop->SetValue(value);
    else
    {
        prop = static_cast<SgPropPointList*>(SgProp::CreateProperty(id));
        prop->SetValue(value);
        Add(prop);
    }
}

void SgNode::SetListProp(SgPropID id, const SgPointSet& value)
{
    SgVector<SgPoint> valueList;
    value.ToVector(&valueList);
    SetListProp(id, valueList);
}

void SgNode::CopyAllPropsFrom(const SgNode& sourceNode)
{
    for (SgPropListIterator it(sourceNode.Props()); it; ++it)
    {
        SgProp* copy = (*it)->Duplicate();
        Add(copy);
    }
}

SgProp* SgNode::CopyPropFrom(const SgNode& sourceNode, SgPropID id)
{
    SgProp* sourceProp = sourceNode.Get(id);
    if (sourceProp)
    {
        SgProp* copy = sourceProp->Duplicate();
        Add(copy);
        return copy;
    }
    else
        return 0;
}

SgProp* SgNode::AddMoveProp(SgMove move, SgBlackWhite player)
{
    SG_ASSERT_BW(player);
    SgPropID id =
        (player == SG_BLACK) ? SG_PROP_MOVE_BLACK : SG_PROP_MOVE_WHITE;
    SgPropMove* moveProp = new SgPropMove(id, move);
    Add(moveProp);
    return moveProp;
}

SgBlackWhite SgNode::Player() const
{
    SG_ASSERT(Get(SG_PROP_PLAYER));
    return static_cast<SgPropPlayer*>(Get(SG_PROP_PLAYER))->Value();
}

int SgNode::CountNodes(bool fSetPropOnThisNode)
{
    int n = 1;
    for (SgSonNodeIterator son(this); son; ++son)
        n += (*son)->CountNodes(/*fSetPropOnThisNode*/false);
    // Set property only on nodes that have at least two sons.
    if (fSetPropOnThisNode || IsBranchPoint())
        SetIntProp(SG_PROP_NUM_NODES, n);
    return n;
}

#ifndef NDEBUG
int SgNode::s_alloc = 0;

int SgNode::s_free = 0;

void SgNode::GetStatistics(int* numAlloc, int* numUsed)
{
    *numAlloc = s_alloc;
    *numUsed = s_alloc - s_free;
}
#endif

void SgNode::MemCheck()
{
    SG_ASSERT(s_alloc == s_free);
}

string SgNode::TreeIndex(const SgNode* node)
{
    ostringstream s;
    if (! node)
        s << "NIL";
    else
    {
        SgNode* father = node->Father();
        if (! father)
            s << '1';
        else
        {
            s << TreeIndex(father) << '.';
            SgNode* son = father->LeftMostSon();
            int index = 1;
            while ((son != node) && son)
            {
                ++index;
                son = son->RightBrother();
            }
            if (son == node)
                s << index;
            else
                SG_ASSERT(false);
        }
    }
    return s.str();
}

//----------------------------------------------------------------------------

SgNodeIterator::SgNodeIterator(SgNode* rootOfSubtree, bool postOrder)
    : m_postOrder(postOrder),
      m_rootOfSubtree(rootOfSubtree)
{
    m_nextNode = m_rootOfSubtree;
    if (m_postOrder)
    {
        while (m_nextNode->HasSon())
            m_nextNode = m_nextNode->LeftMostSon();
    }
}

SgNodeConstIterator::SgNodeConstIterator(const SgNode* rootOfSubtree,
                                         bool postOrder)
    : m_postOrder(postOrder),
      m_rootOfSubtree(rootOfSubtree)
{
    m_nextNode = m_rootOfSubtree;
    if (m_postOrder)
    {
        while (m_nextNode->HasSon())
            m_nextNode = m_nextNode->LeftMostSon();
    }
}

bool SgNodeIterator::Next()
{
    if (m_nextNode)
    {
        if (m_postOrder)
        {
            if (m_nextNode == m_rootOfSubtree)
                m_nextNode = 0;
            else if (m_nextNode->HasRightBrother())
            {
                m_nextNode = m_nextNode->RightBrother();
                while (m_nextNode->HasSon())
                    m_nextNode = m_nextNode->LeftMostSon();
            }
            else
            {
                SG_ASSERT(m_nextNode->HasFather());
                m_nextNode = m_nextNode->Father();
            }
        }
        else
        {
            m_nextNode = m_nextNode->NextDepthFirst();
            if (m_nextNode == m_rootOfSubtree)
                m_nextNode = 0;
        }
        return true;
    }
    else
        return false;
}

bool SgNodeConstIterator::Next()
{
    if (m_nextNode)
    {
        if (m_postOrder)
        {
            if (m_nextNode == m_rootOfSubtree)
                m_nextNode = 0;
            else if (m_nextNode->HasRightBrother())
            {
                m_nextNode = m_nextNode->RightBrother();
                while (m_nextNode->HasSon())
                    m_nextNode = m_nextNode->LeftMostSon();
            }
            else
            {
                SG_ASSERT(m_nextNode->HasFather());
                m_nextNode = m_nextNode->Father();
            }
        }
        else
        {
            m_nextNode = m_nextNode->NextDepthFirst();
            if (m_nextNode == m_rootOfSubtree)
                m_nextNode = 0;
        }
        return true;
    }
    else
        return false;
}

//----------------------------------------------------------------------------

