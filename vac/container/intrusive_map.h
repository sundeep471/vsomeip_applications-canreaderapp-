/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2020 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/*!        \file  intrusive_map.h
 *        \brief  The header file of intrusive map is an implementation of a key-value-storage where the
 *                key is stored inside the value object.
 *
 *      \details  Implement the tree-like functions, for example, find parent node, find left/right nodes
 *                erase node, and insert node.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_MAP_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_MAP_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "vac/testing/test_adapter.h"

namespace vac {
namespace container {

// VECTOR Next Construct Metric-OO.WMC.One: MD_VAC_Metric-OO.WMC.One
/*!
 * \brief  Type for nodes of the intrusive map.
 *         This serves as a pair of key and value to be inserted in the map as a node.
 * \tparam key Type for key in the pair.
 * \tparam T Type of the value in the pair.
 */
template <typename key, typename T>
class IntrusiveMapNode {
 public:
  /*!
   * \brief Type of contained key.
   */
  using key_type = key;

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief Type of compare result.
   */
  using CompareType = std::int32_t;

  /*!
   * \brief Constructor for a node without a map.
   */
  IntrusiveMapNode() noexcept = default;

  /*!
   * \brief Default copy constructor deleted.
   */
  IntrusiveMapNode(IntrusiveMapNode const&) = delete;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   */
  IntrusiveMapNode& operator=(IntrusiveMapNode const&) & = delete;

  /*!
   * \brief Default move constructor deleted.
   */
  IntrusiveMapNode(IntrusiveMapNode&&) = delete;

  /*!
   * \brief  Default move assignment operator deleted.
   * \return
   */
  IntrusiveMapNode& operator=(IntrusiveMapNode&&) & = delete;

  /*!
   * \brief  Compare the keys of this element to another key.
   * \return Negative value if new key is less than the other current node key, zero if the both keys are equal,
   *         positive value if new key is greater than the other current node key.
   */
  virtual CompareType KeyCompare(key_type) const noexcept = 0;

  /*!
   * \brief  Compare the keys of this element to the key obtained from another element.
   *         This function is necessary as the IntrusiveMap class needs to compare IntrusiveMapNode objects. However, as
   *         it does not know how the IntrusiveMapNode stores its key, it cannot extract the key for comparison using
   *         KeyCompare(key_type).
   * \return Negative value if new key is less than the other current node key, zero if the both keys are equal,
   *         positive value if new key is greater than the other current node key.
   */
  virtual CompareType KeyCompare(T const&) const noexcept = 0;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor that removes the node from a map, if it is contained in one.
   */
  virtual ~IntrusiveMapNode() noexcept { EraseFromMap(); }

  /*!
   * \brief Erase the node from a map.
   */
  void EraseFromMap() noexcept {
    // Both child nodes present.
    if ((left_ != nullptr) && (right_ != nullptr)) {
      // Find the node with maximum key value in the left sub tree.
      IntrusiveMapNode<key, T>* const temp{FindMaxLeft()};

      // Erase temp from the map.
      temp->EraseNodeWithOneOrNoChild();

      // Adjust the pointers of the node and parent node.
      temp->SetLeft(left_);
      temp->SetRight(right_);
      temp->SetParent(parent_);

      if (parent_->Right() == this) {
        parent_->SetRight(temp);
      }
      if (parent_->Left() == this) {
        parent_->SetLeft(temp);
      }

      // Adjust the left and right parent pointers.
      if (left_ != nullptr) {
        left_->parent_ = temp;
      }
      // The sorted map node (except for the rightest node) will always contains the right hand child.
      // VECTOR NL AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_pointerIsNullAtDereference
      right_->parent_ = temp;

      // Reset links to parent and child.
      left_ = nullptr;
      right_ = nullptr;
      parent_ = nullptr;

    } else {
      // If node has one or no child
      EraseNodeWithOneOrNoChild();
    }
  }

  // VECTOR NC AutosarC++17_10-M5.2.3: MD_VAC_M5.2.3_castFromPolymorphicBaseClassToDerivedClass
  // VECTOR NC VectorC++-V5.2.3: MD_VAC_V5-2-3_castFromPolymorphicBaseClassToDerivedClass
  /*!
   * \brief  Get the contained element.
   * \return The contained element.
   */
  T* GetSelf() noexcept {
    static_assert(std::is_base_of<IntrusiveMapNode<key, T>, T>::value, "T must derive from IntrusiveMapNode");
    return static_cast<T*>(this);
  }

  // VECTOR NC AutosarC++17_10-M5.2.3: MD_VAC_M5.2.3_castFromPolymorphicBaseClassToDerivedClass
  // VECTOR NC VectorC++-V5.2.3: MD_VAC_V5-2-3_castFromPolymorphicBaseClassToDerivedClass
  /*!
   * \brief  Get the contained element.
   * \return The contained element.
   */
  T const* GetSelf() const noexcept {
    static_assert(std::is_base_of<IntrusiveMapNode<key, T>, T>::value, "T must derive from IntrusiveMapNode");
    return static_cast<T const*>(this);
  }

  /*!
   * \brief  Returns the pointer to left node.
   * \return Pointer to the left child node.
   */
  IntrusiveMapNode* Left() noexcept { return left_; }

  /*!
   * \brief  Returns const pointer to left node.
   * \return Const pointer to the left child node.
   */
  IntrusiveMapNode const* Left() const noexcept { return left_; }

  /*!
   * \brief  Returns the pointer to right node.
   * \return Pointer to the right child node.
   */
  IntrusiveMapNode* Right() noexcept { return right_; }

  /*!
   * \brief  Returns const pointer to right node.
   * \return Const pointer to the right child node.
   */
  IntrusiveMapNode const* Right() const noexcept { return right_; }

  /*!
   * \brief  Returns the pointer to parent node.
   * \return Pointer to the parent node.
   */
  IntrusiveMapNode* Parent() noexcept { return parent_; }

  /*!
   * \brief  Returns const pointer to parent node.
   * \return Const pointer to the parent node.
   */
  IntrusiveMapNode const* Parent() const noexcept { return parent_; }

  /*!
   * \brief Set the left element.
   * \param new_elem Pointer to an IntrusiveMapNode.
   */
  void SetLeft(IntrusiveMapNode* new_elem) noexcept { left_ = new_elem; }

  /*!
   * \brief Set the right element.
   * \param new_elem Pointer to an IntrusiveMapNode.
   */
  void SetRight(IntrusiveMapNode* new_elem) noexcept { right_ = new_elem; }

  /*!
   * \brief Set the parent element.
   * \param parent Pointer to an IntrusiveMapNode.
   */
  void SetParent(IntrusiveMapNode* parent) noexcept { parent_ = parent; }

  /*!
   * \brief  Find min element from the right subtree.
   * \return Element from right subtree with minimum key or nullptr if the right subtree is empty.
   */
  IntrusiveMapNode const* FindMinRight() const noexcept {
    IntrusiveMapNode const* min_right{this};
    if (min_right->Right() == nullptr) {
      min_right = nullptr;
    } else {
      min_right = min_right->Right();
      while (min_right->Left() != nullptr) {
        min_right = min_right->Left();
      }
    }
    return min_right;
  }

  /*!
   * \brief  Find max element from the left subtree.
   * \return Element from left subtree with maximum key, or nullptr if the left subtree is emtpy.
   */
  IntrusiveMapNode const* FindMaxLeft() const noexcept {
    IntrusiveMapNode const* max_left{this};
    if (max_left->Left() == nullptr) {
      max_left = nullptr;
    } else {
      max_left = max_left->Left();
      while (max_left->Right() != nullptr) {
        max_left = max_left->Right();
      }
    }
    return max_left;
  }

  /*!
   * \brief  Find a parent node with immediate large key value.
   * \return Parent with larger key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode const* FindLargerParent() const noexcept {
    IntrusiveMapNode const* drag{this};
    IntrusiveMapNode const* larger_parent{this};
    // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
    while ((larger_parent != nullptr) && (larger_parent->Left() != drag)) {
      // While loop terminates when reaching the end() element or when coming in to the parent from a left child.
      drag = larger_parent;
      larger_parent = larger_parent->Parent();
    }  // At this point, larger_parent is either the larger parent or end().
    return larger_parent;
  }

  /*!
   * \brief  Find a parent node with immediate smaller key value.
   * \return Parent with smaller key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode const* FindSmallerParent() const noexcept {
    IntrusiveMapNode const* drag{this};
    IntrusiveMapNode const* smaller_parent{this};
    // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
    while ((smaller_parent != nullptr) && (smaller_parent->Right() != drag)) {
      // While loop terminates when reaching the end() element or when coming in to the parent from a right child
      drag = smaller_parent;
      smaller_parent = smaller_parent->Parent();
    }  // At this point, larger_parent is either the larger parent or end().
    return smaller_parent;
  }

  /*!
   * \brief  Find max element from the left subtree.
   * \return Element from left subtree with maximum key.
   */
  IntrusiveMapNode* FindMaxLeft() noexcept {
    // VECTOR Next Line AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindMaxLeft());
  }

  /*!
   * \brief  Find min element from the right subtree.
   * \return Element from right subtree with minimum key.
   */
  IntrusiveMapNode* FindMinRight() noexcept {
    // VECTOR Next Line AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindMinRight());
  }

  /*!
   * \brief  Find a parent node with immediate large key value.
   * \return Parent with larger key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode* FindLargerParent() noexcept {
    // VECTOR Next Line AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindLargerParent());
  }

  /*!
   * \brief  Find a parent node with immediate smaller key value.
   * \return Parent with smaller key value or nullptr in case such a parent is not found.
   */
  IntrusiveMapNode* FindSmallerParent() noexcept {
    // VECTOR Next Line AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    return const_cast<IntrusiveMapNode*>(static_cast<IntrusiveMapNode const*>(this)->FindSmallerParent());
  }

 private:
  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_pointerIsNullAtDereference
  /*! \brief Helper function to update the left and right parent pointers */
  void UpdateParentPointers() noexcept {
    if (this->HasParent()) {
      if (this == parent_->right_) {
        parent_->right_ = right_;
        if (left_ != nullptr) {
          parent_->right_ = left_;
        }
      }
      if (this == parent_->left_) {
        parent_->left_ = right_;
        if (left_ != nullptr) {
          parent_->left_ = left_;
        }
      }
    }
  }

  /*! \brief Helper function to adjust the left and right child pointers */
  void UpdateChildPointers() noexcept {
    if (left_ != nullptr) {
      left_->parent_ = parent_;
    }
    if (right_ != nullptr) {
      right_->parent_ = parent_;
    }
  }
  /*!
   * \brief Erase the node from a map that has either no or exactly one child.
   */
  void EraseNodeWithOneOrNoChild() noexcept {
    assert(((left_ == nullptr) || (right_ == nullptr)));  // Deleting a node that has only one or no child.
    UpdateParentPointers();
    UpdateChildPointers();
    // Adjust the node pointers
    left_ = nullptr;
    right_ = nullptr;
    parent_ = nullptr;
  }

  /*!
   * \brief  Determines if the 'this' node has a parent node.
   * \return True if a parent node exists, else false.
   */
  bool HasParent() const noexcept { return parent_ != nullptr; }

  /*!
   * \brief Pointer to the left child node.
   */
  IntrusiveMapNode* left_{nullptr};

  /*!
   * \brief Pointer to the right child node.
   */
  IntrusiveMapNode* right_{nullptr};

  /*!
   * \brief Pointer to the parent node.
   */
  IntrusiveMapNode* parent_{nullptr};
};

/*!
 * \brief Type for end node of the intrusive map.
 */
template <typename Key, typename T>
class EndNode final : public IntrusiveMapNode<Key, T> {
 public:
  /*!
   * \brief Constructor.
   */
  using IntrusiveMapNode<Key, T>::IntrusiveMapNode;

  /*!
   * \brief Type of compare result.
   */
  using CompareType = typename IntrusiveMapNode<Key, T>::CompareType;

  /*!
   * \brief  Compares the  key value with the node current node value.
   * \return Always returns 1 because this is the root node with only left subtree so the key is considered to be
   *         always greater then any other key in the map.
   */
  CompareType KeyCompare(Key) const noexcept final { return 1; }

  /*!
   * \brief  Compares the  key value with the node current node value.
   * \return Always returns 1 because this is the root node with only left subtree so the key is considered to be
   *         always greater then any other key in the map.
   */
  CompareType KeyCompare(T const&) const noexcept final { return 1; }
};

/*!
 * \brief Iterator for this IntrusiveMap.
 * \trace CREQ-160859
 */
template <typename key, typename T>
class IntrusiveMapIterator final {
 public:
  /*! \brief Category. */
  using iterator_category = std::bidirectional_iterator_tag;
  /*! \brief Value type. */
  using value_type = T;
  /*! \brief Difference type. */
  using difference_type = std::ptrdiff_t;
  /*! \brief Pointer type. */
  using pointer = T*;
  /*! \brief Reference type. */
  using reference = T&;

  /*!
   * \brief Constructor for an iterator.
   * \param node The Map from which to construct the iterator.
   */
  explicit IntrusiveMapIterator(IntrusiveMapNode<key, T>* node) noexcept : node_(node) {}

  /*!
   * \brief  Return a reference to the contained element.
   * \return A reference to the element type.
   */
  T& operator*() noexcept { return *(this->GetMapNode()->GetSelf()); }

  /*!
   * \brief  Return a reference to the contained element.
   * \return A const reference to the element type.
   */
  T const& operator*() const noexcept { return *(this->GetMapNode()->GetSelf()); }

  /*!
   * \brief  Get the map node .
   * \return The contained pointer to the map node.
   */
  IntrusiveMapNode<key, T>* GetMapNode() noexcept { return node_; }

  /*!
   * \brief  Get the map node .
   * \return The contained pointer to the map node.
   */
  IntrusiveMapNode<key, T> const* GetMapNode() const noexcept { return node_; }

  /*!
   * \brief  Compare two iterators for equality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to the same map node.
   */
  bool operator==(IntrusiveMapIterator const& other) const noexcept { return node_ == other.node_; }

  /*!
   * \brief  Compare two iterators for inequality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to different map nodes.
   */
  bool operator!=(IntrusiveMapIterator const& other) const noexcept { return node_ != other.node_; }

  /*!
   * \brief  Increment the iterator by one element.
   * \return IntrusiveMapIterator to the element with immediate higher key value.
   */
  IntrusiveMapIterator& operator++() noexcept {
    IntrusiveMapNode<key, T>* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMinRight();

      // If Min right node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindLargerParent();
      }
    }
    if (temp_elem != nullptr) {
      // Avoid leaving the tree, remain on the end() element.
      node_ = temp_elem;
    }
    return *this;
  }

  /*!
   * \brief  Decrement the iterator by one element.
   * \return IntrusiveMapIterator to the element with an immediate lower key value.
   */
  IntrusiveMapIterator& operator--() noexcept {
    IntrusiveMapNode<key, T>* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMaxLeft();

      // If Max Left node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindSmallerParent();
      }
    }
    node_ = temp_elem;
    return *this;
  }

 private:
  /*!
   * \brief Pointer to the map element.
   */
  IntrusiveMapNode<key, T>* node_;
};

/*!
 * \brief Const Iterator for this IntrusiveMap.
 */
template <typename key, typename T>
class ConstIntrusiveMapIterator final {
 public:
  /*! \brief Category. */
  using iterator_category = std::bidirectional_iterator_tag;
  /*! \brief Value type. */
  using value_type = T;
  /*! \brief Difference type. */
  using difference_type = std::ptrdiff_t;
  /*! \brief Pointer type. */
  using pointer = T const*;
  /*! \brief Reference type. */
  using reference = T const&;

  // VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
  /*!
   * \brief Constructor for an iterator.
   * \param node The Map from which to construct the iterator.
   */
  explicit ConstIntrusiveMapIterator(IntrusiveMapNode<key, T> const* node) noexcept : node_(node) {}

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief Constructor for const_iterator from an iterator.
   * \param it The iterator from which to construct the const_iterator.
   */
  ConstIntrusiveMapIterator(IntrusiveMapIterator<key, T> const& it) noexcept  // NOLINT(runtime/explicit)
      : node_(it.GetMapNode()) {}

  /*!
   * \brief  Return a reference to the contained element.
   * \return A const reference to the element type.
   */
  reference operator*() const noexcept { return *(this->GetMapNode()->GetSelf()); }

  /*!
   * \brief  Get the map node .
   * \return The contained pointer to the map node.
   */
  IntrusiveMapNode<key, T> const* GetMapNode() const noexcept { return node_; }

  /*!
   * \brief  Compare two iterators for equality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to the same map node.
   */
  bool operator==(ConstIntrusiveMapIterator const& other) const noexcept { return node_ == other.node_; }

  /*!
   * \brief  Compare two iterators for inequality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to different map nodes.
   */
  bool operator!=(ConstIntrusiveMapIterator const& other) const noexcept { return node_ != other.node_; }

  /*!
   * \brief  Increment the iterator by one element.
   * \return IntrusiveMapIterator to the element with immediate higher key value.
   */
  ConstIntrusiveMapIterator& operator++() noexcept {
    IntrusiveMapNode<key, T> const* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMinRight();

      // If Min right node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindLargerParent();
      }
    }
    node_ = temp_elem;
    return *this;
  }

  /*!
   * \brief  Decrement the iterator by one element.
   * \return IntrusiveMapIterator to the element with an immediate lower key value.
   */
  ConstIntrusiveMapIterator& operator--() noexcept {
    IntrusiveMapNode<key, T> const* temp_elem{node_};
    if (node_ != nullptr) {
      temp_elem = node_->FindMaxLeft();

      // If Max Left node not found.
      if (temp_elem == nullptr) {
        temp_elem = node_->FindSmallerParent();
      }
    }
    node_ = temp_elem;
    return *this;
  }

 private:
  /*!
   * \brief Pointer to the map element.
   */
  IntrusiveMapNode<key, T> const* node_;
};

/*!
 * \brief Class to implement an IntrusiveMap.
 * \trace CREQ-158591
 */
template <typename key, typename T>
class IntrusiveMap final {
 public:
  /*!
   * \brief Typedef for the contained element.
   */
  using value_type = T;

  /*!
   * \brief Typedef for the iterator type of this map.
   */
  using iterator = IntrusiveMapIterator<key, T>;

  /*!
   * \brief Typedef for the iterator type of this map.
   */
  using const_iterator = ConstIntrusiveMapIterator<key, T>;

  /*!
   * \brief Type of compare result.
   */
  using CompareType = typename IntrusiveMapNode<key, T>::CompareType;

  /*!
   * \brief Constructor.
   */
  IntrusiveMap() noexcept : map_() { map_.SetParent(nullptr); }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor.
   */
  ~IntrusiveMap() noexcept {
    // Release all elements from the map starting from the smallest key member.
    while (!empty()) {
      erase(begin());
    }
  }

  /*!
   * \brief Deleted copy constructor.
   */
  IntrusiveMap(IntrusiveMap const&) = delete;
  /*!
   * \brief Deleted move constructor.
   */
  IntrusiveMap(IntrusiveMap&&) = delete;
  /*!
   * \brief  Deleted copy assignent.
   * \return
   */
  IntrusiveMap& operator=(IntrusiveMap const&) & = delete;
  /*!
   * \brief  Deleted move assignent.
   * \return
   */
  IntrusiveMap& operator=(IntrusiveMap&&) & = delete;

  /*!
   * \brief  Remove an element from map.
   * \param  erase_key Key to be erased.
   * \return Number of elements removed.
   */
  std::size_t erase(key const& erase_key) noexcept {
    std::size_t deleted_count{0};
    iterator itr{find(erase_key)};

    // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
    if ((map_.Left() != nullptr) && (itr != end())) {
      IntrusiveMapNode<key, T>* const elem{itr.GetMapNode()};
      // Erase the node.
      elem->EraseFromMap();
      deleted_count++;
    }
    return deleted_count;
  }

  /*!
   * \brief Remove an element from map.
   * \param elem Iterator to the element to be erased.
   */
  void erase(iterator elem) noexcept {
    // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
    if ((!empty()) && (elem != end())) {
      if (elem.GetMapNode() != nullptr) {
        elem.GetMapNode()->EraseFromMap();
      }
    }
  }

  /*!
   * \brief  Determine whether the map is currently empty.
   * \return True if the map is empty. False if the map has at least one element.
   */
  bool empty() const noexcept { return (map_.Left() == nullptr); }

  /*!
   * \brief  Insert a new node (key value pair) into the map at appropriate position.
   * \param  node Element to be inserted in the map.
   * \return Pair of iterator and boolean that indicates if the item was added (true)
   *         or an item with the same key already exists (false).
   */
  std::pair<iterator, bool> insert(IntrusiveMapNode<key, T>* node) noexcept {
    bool is_inserted{false};
    if (map_.Left() == nullptr) {
      map_.SetLeft(node);
      if (node != nullptr) {
        node->SetParent(&map_);
        is_inserted = true;
      }
    } else {
      if (node != nullptr) {
        T const* const self{node->GetSelf()};
        IntrusiveMapNode<key, T>* const temp_node{SearchNode(*self)};
        CompareType const result{temp_node->KeyCompare(*self)};

        if (result > 0) {
          temp_node->SetRight(node);
          node->SetParent(temp_node);
          is_inserted = true;
        } else if (result < 0) {
          temp_node->SetLeft(node);
          node->SetParent(temp_node);
          is_inserted = true;
        } else {
          // Get the node that prevented the insertion.
          node = temp_node;
        }
      }
    }
    return std::make_pair(iterator(node), is_inserted);
  }

  /*!
   * \brief  Find node in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a end_ node.
   */
  iterator find(key const& find_key) noexcept {
    iterator ret_value{end()};
    IntrusiveMapNode<key, T>* const node{SearchNode(find_key)};
    // VECTOR Next Line AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
    if ((node != nullptr) && (node->KeyCompare(find_key) == 0)) {
      ret_value = iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Find node in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a end_ node.
   */
  const_iterator find(key const& find_key) const noexcept {
    const_iterator ret_value{cend()};
    IntrusiveMapNode<key, T> const* const node{SearchNode(find_key)};
    if ((node != nullptr) && (node->KeyCompare(find_key) == 0)) {
      ret_value = const_iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Get iterator to smallest key node.
   * \return Iterator to Intrusive map.
   */
  iterator begin() noexcept {
    iterator ret_value{end()};
    if (!empty()) {
      IntrusiveMapNode<key, T>* node{map_.Left()};
      while (node->Left() != nullptr) {
        node = node->Left();
      }
      ret_value = iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Get const iterator to smallest key node.
   * \return Const iterator to Intrusive map.
   */
  const_iterator cbegin() const noexcept {
    const_iterator ret_value{cend()};
    if (!empty()) {
      IntrusiveMapNode<key, T> const* node{map_.Left()};
      while (node->Left() != nullptr) {
        node = node->Left();
      }
      ret_value = const_iterator(node);
    }
    return ret_value;
  }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  iterator end() noexcept { return iterator(&map_); }

  /*!
   * \brief  Get iterator to end element.
   * \return Iterator to end element.
   */
  const_iterator cend() const noexcept { return const_iterator(&map_); }

 private:
  /*!
   * \brief  Search for a node position in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a node just previous to the matching key.
   */
  template <class KeyOrNode>
  IntrusiveMapNode<key, T> const* SearchNode(KeyOrNode const& find_key) const noexcept {
    IntrusiveMapNode<key, T> const* temp{map_.Left()};
    IntrusiveMapNode<key, T> const* prev{nullptr};
    while (temp != nullptr) {
      prev = temp;
      CompareType const result{temp->GetSelf()->KeyCompare(find_key)};
      if (result > 0) {
        temp = temp->Right();
      } else if (result < 0) {
        temp = temp->Left();
      } else {
        break;
      }
    }
    return prev;
  }

  // VECTOR Next Construct AutosarC++17_10-M9.3.3: MD_VAC_M9.3.3_constCastReducesCodeDuplication
  // VECTOR Next Construct AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
  /*!
   * \brief  Search for a node position in map with the input key.
   * \param  find_key Key to to searched for.
   * \return Iterator to a node matching the key or a node just previous to the matching key.
   */
  template <class KeyOrNode>
  IntrusiveMapNode<key, T>* SearchNode(KeyOrNode const& find_key) noexcept {
    return const_cast<IntrusiveMapNode<key, T>*>(const_cast<IntrusiveMap const*>(this)->SearchNode(find_key));
  }

  /*!
   * \brief Pointer to the map.
   *        Map_ is the root node and the actual map starts from the map_.left. map_.right will always be nullptr.
   *        For an empty map the map_.left will be nullptr.
   */
  EndNode<key, T> map_;

  FRIEND_TEST(IntrusiveMap, IteratorOperationsIncrementRightSubTree);
  FRIEND_TEST(IntrusiveMap, IteratorOperationsIncrementLeftSubTree);
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_MAP_H_
