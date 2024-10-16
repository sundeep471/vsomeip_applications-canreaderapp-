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
/*!        \file  intrusive_list.h
 *        \brief  The header file of the intrusive list.
 *
 *      \details  Implement the functions, for example, access the previous/next node, insert a node and
 *                remove a node from the list. In addition, popping elements from the front or back is included.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_LIST_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_LIST_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>
#include <iterator>
#include <utility>

namespace vac {
namespace container {

/*!
 * \brief Forward-Declare IntrusiveList so that it can be friended.
 */
template <typename T>
class IntrusiveList;

/*!
 * \brief Type for nodes of the intrusive list.
 *        Derive from this type to create an obtain an object that can be a list member.
 *        T should be the type of your object.
 */
template <typename T>
class IntrusiveListNode {
 public:
  /*!
   * \brief Constructor for a node without a list.
   */
  constexpr IntrusiveListNode() noexcept : prev_(this), next_(this) {
    static_assert(std::is_base_of<IntrusiveListNode<T>, T>::value, "T must be a descendant of IntrusiveListNode<T>");
  }

  /*!
   * \brief Default copy constructor deleted.
   */
  IntrusiveListNode(IntrusiveListNode const&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   */
  IntrusiveListNode& operator=(IntrusiveListNode const&) & = delete;
  /*!
   * \brief Default move constructor deleted.
   */
  IntrusiveListNode(IntrusiveListNode&&) = delete;
  /*!
   * \brief  Default move assignment operator deleted.
   * \return
   */
  IntrusiveListNode& operator=(IntrusiveListNode&&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor that removes the node from a list, if it is contained in one.
   */
  virtual ~IntrusiveListNode() noexcept { EraseFromList(); }

  /*!
   * \brief   Erase the node from a list.
   * \details Does nothing if the node is not part of a list.
   *          Note that this method is used internally by the list implementation. It may be called by users,
   *          but typically leads to surprising semantics. It is recommended to use l.erase(it) instead.
   */
  void EraseFromList() noexcept {
    if (prev_ != this) {
      prev_->next_ = next_;
      next_->prev_ = prev_;
      next_ = this;
      prev_ = this;
    }
  }

  // VECTOR Next Construct AutosarC++17_10-M5.2.3: MD_VAC_M5.2.3_castFromPolymorphicBaseClassToDerivedClass
  // VECTOR Next Construct VectorC++-V5.2.3: MD_VAC_V5-2-3_castFromPolymorphicBaseClassToDerivedClass
  /*!
   * \brief  Get the contained element.
   * \return The contained element.
   */
  T* GetSelf() noexcept {
    static_assert(std::is_base_of<IntrusiveListNode<T>, T>::value, "T must derive from IntrusiveListNode");
    return static_cast<T*>(this);
  }

  // VECTOR Disable VectorC++-V5.2.3: MD_VAC_V5-2-3_castFromPolymorphicBaseClassToDerivedClass
  // VECTOR Disable AutosarC++17_10-M5.2.3: MD_VAC_M5.2.3_castFromPolymorphicBaseClassToDerivedClass
  /*!
   * \brief  Get the contained element.
   * \return The contained element.
   */
  T const* GetSelf() const noexcept {
    static_assert(std::is_base_of<IntrusiveListNode<T>, T>::value, "T must derive from IntrusiveListNode");
    return static_cast<T const*>(this);
  }
  // VECTOR Enable AutosarC++17_10-M5.2.3
  // VECTOR Enable VectorC++-V5.2.3

  /*!
   * \brief  Get the next element.
   * \return The next element.
   */
  IntrusiveListNode* Next() noexcept { return next_; }

  /*!
   * \brief  Get the next element.
   * \return The next element.
   */
  IntrusiveListNode const* Next() const noexcept { return next_; }

  /*!
   * \brief  Get the previous element.
   * \return The previous element.
   */
  IntrusiveListNode* Prev() noexcept { return prev_; }

  /*!
   * \brief  Get the previous element.
   * \return The previous element.
   */
  IntrusiveListNode const* Prev() const noexcept { return prev_; }

  /*!
   * \brief Insert an element after the given element.
   * \param newElem The element to insert.
   */
  void InsertAfter(IntrusiveListNode& newElem) noexcept {
    // Set pointer from this element to the new successor & obtain the old successor.
    IntrusiveListNode* const successor{this->AppendAfter(newElem)};
    // Set pointer from the new element to its new successor (this' former successor).
    static_cast<void>(newElem.AppendAfter(*successor));
  }

 private:
  // VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
  /*!
   * \brief  Insert the newElem after this node without modifying newElem's successors.
   *         Note: This Method will destroy the coherence of a list. At the very least,
   *         the list will no longer be a ring.
   *         The caller must make sure to put the list back into a valid state.
   * \param  newElem The new element to append.
   * \return The former successor of this node.
   */
  IntrusiveListNode* AppendAfter(IntrusiveListNode& newElem) noexcept {
    IntrusiveListNode* const successor{next_};

    // Forward pointers.
    this->next_ = &newElem;

    // Reverse pointers.
    newElem.prev_ = this;

    return successor;
  }

  /*!
   * \brief Pointer to the previous list element.
   */
  IntrusiveListNode* prev_;

  /*!
   * \brief Pointer to the following list element.
   */
  IntrusiveListNode* next_;

  /*!
   * \brief  Friend class declaration of IntrusiveList.
   * \tparam T Type of the IntrusiveList.
   */
  friend class IntrusiveList<T>;  // VECTOR SL AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend
};

/*!
 * \brief Iterator for this IntrusiveList.
 * \trace CREQ-160859
 */
template <typename T>
class IntrusiveListIterator final {
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
   * \param node The list from which to construct the iterator.
   */
  explicit IntrusiveListIterator(IntrusiveListNode<T>* node) noexcept : node_(node) {}

  /*!
   * \brief  Advance the iterator by one element.
   * \return A reference to the current iterator.
   */
  IntrusiveListIterator& operator++() noexcept {
    node_ = node_->Next();
    return *this;
  }

  /*!
   * \brief  Back the iterator up by one element.
   * \return A reference to the current iterator.
   */
  IntrusiveListIterator& operator--() noexcept {
    node_ = node_->Prev();
    return *this;
  }

  /*!
   * \brief  Get the list node.
   * \return A reference to the list node pointed to by this iterator.
   */
  reference operator*() const noexcept { return *node_->GetSelf(); }

  /*!
   * \brief  Get the list node.
   * \return A reference to the list node pointed to by this iterator.
   */
  pointer operator->() const noexcept { return node_->GetSelf(); }

  /*!
   * \brief  Get the list node as list node rather than as list element type.
   *         Works even for IntrusiveList::end().
   * \return The contained pointer to the list node.
   */
  IntrusiveListNode<T>* GetListNode() noexcept { return node_; }

  /*!
   * \brief  Get the list node as list node rather than as list element type.
   *         Works even for IntrusiveList::end().
   * \return The contained pointer to the list node.
   */
  IntrusiveListNode<T> const* GetListNode() const noexcept { return node_; }

  /*!
   * \brief  Compare two iterators for equality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to the same list node.
   */
  bool operator==(IntrusiveListIterator const& other) const noexcept { return node_ == other.node_; }

  /*!
   * \brief  Compare two iterators for inequality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to different list nodes.
   */
  bool operator!=(IntrusiveListIterator const& other) const noexcept { return node_ != other.node_; }

 private:
  /*!
   * \brief Pointer to the list element.
   */
  IntrusiveListNode<T>* node_;
};

/*!
 * \brief Const Iterator for this IntrusiveList.
 */
template <typename T>
class ConstIntrusiveListIterator final {
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
   * \param node The list from which to create the iterator.
   */
  explicit ConstIntrusiveListIterator(IntrusiveListNode<T> const* node) noexcept : node_(node) {}

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief Constructor for const_iterator from an iterator.
   * \param it The iterator from which to construct the const_iterator.
   */
  ConstIntrusiveListIterator(IntrusiveListIterator<T> const& it) noexcept  // NOLINT(runtime/explicit)
      : node_(it.GetListNode()) {}

  /*!
   * \brief  Advance the iterator by one element.
   * \return A reference to the current iterator.
   */
  ConstIntrusiveListIterator& operator++() noexcept {
    node_ = node_->Next();
    return *this;
  }

  /*!
   * \brief  Back the iterator up by one element.
   * \return A reference to the current iterator.
   */
  ConstIntrusiveListIterator& operator--() noexcept {
    node_ = node_->Prev();
    return *this;
  }

  /*!
   * \brief  Get the list node.
   * \return A reference to the list node pointed to by this iterator.
   */
  reference operator*() const noexcept { return *node_->GetSelf(); }

  /*!
   * \brief  Get the list node.
   * \return A reference to the list node pointed to by this iterator.
   */
  pointer operator->() const noexcept { return node_->GetSelf(); }

  /*!
   * \brief  Get the list node as list node rather than as list element type.
   *         Works even for IntrusiveList::end().
   * \return The contained pointer to the list node.
   */
  IntrusiveListNode<T> const* GetListNode() const noexcept { return node_; }

  /*!
   * \brief  Compare two iterators for equality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to the same list node.
   */
  bool operator==(ConstIntrusiveListIterator const& other) const noexcept { return node_ == other.node_; }

  /*!
   * \brief  Compare two iterators for inequality.
   * \param  other Iterator to compare to.
   * \return True if both iterators point to different list nodes.
   */
  bool operator!=(ConstIntrusiveListIterator const& other) const noexcept { return node_ != other.node_; }

 private:
  /*!
   * \brief Pointer to the list element.
   */
  IntrusiveListNode<T> const* node_;
};

// VECTOR Next Construct Metric-OO.WMC.One: MD_VAC_Metric-OO.WMC.One
/*!
 * \brief Class to implement an IntrusiveList.
 *        List Nodes must inherit from IntrusiveListNode<T>.
 * \trace CREQ-158590
 */
template <typename T>
class IntrusiveList final {
 public:
  /*!
   * \brief Typedef for the contained element.
   */
  using value_type = T;

  /*!
   * \brief Typedef for the iterator type of this list.
   */
  using iterator = IntrusiveListIterator<T>;

  /*!
   * \brief Typedef for the iterator type of this list.
   */
  using const_iterator = ConstIntrusiveListIterator<T>;

  /*!
   * \brief Default constructor.
   */
  IntrusiveList() noexcept : list_() {}

  /*!
   * \brief Default copy constructor deleted.
   */
  IntrusiveList(IntrusiveList const&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   */
  IntrusiveList& operator=(IntrusiveList const&) & = delete;
  /*!
   * \brief Default move constructor deleted.
   */
  IntrusiveList(IntrusiveList&&) = delete;
  /*!
   * \brief  Default move assignment operator deleted.
   * \return
   */
  IntrusiveList& operator=(IntrusiveList&&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor that releases all elements from the list.
   */
  ~IntrusiveList() noexcept {
    while (!empty()) {
      static_cast<void>(pop_front());
    }
  }

  /*!
   * \brief Insert an element at the front of the list.
   * \param elem The new element to add in front.
   */
  void push_front(T& elem) noexcept { list_.InsertAfter(elem); }

  /*!
   * \brief  Remove an element from the front of the list and return it.
   * \return The first element from the list.
   */
  IntrusiveListNode<T>* pop_front() noexcept {
    IntrusiveListNode<T>* ret_value{nullptr};
    if (!empty()) {
      ret_value = list_.Next();
      ret_value->EraseFromList();
    }
    return ret_value;
  }

  /*!
   * \brief  Returns a reference to the first element of the intrusive list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to an IntrusiveListNode.
   */
  IntrusiveListNode<T>& front() noexcept { return *list_.Next(); }

  /*!
   * \brief  Returns a reference to the first element of the intrusive list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to an IntrusiveListNode.
   */
  IntrusiveListNode<T> const& front() const noexcept { return *list_.Next(); }

  /*!
   * \brief Insert an element at the back of the list.
   * \param elem The element to add in the back.
   */
  void push_back(T& elem) noexcept { list_.Prev()->InsertAfter(elem); }

  /*!
   * \brief  Remove an element from the back of the list and return it.
   * \return The last element from the list.
   */
  IntrusiveListNode<T>* pop_back() noexcept {
    IntrusiveListNode<T>* ret_value{nullptr};
    if (!empty()) {
      ret_value = list_.Prev();
      ret_value->EraseFromList();
    }
    return ret_value;
  }

  /*!
   * \brief  Returns a reference to the last element of the intrusive list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to an IntrusiveListNode.
   */
  IntrusiveListNode<T>& back() noexcept { return *list_.Prev(); }

  /*!
   * \brief  Returns a reference to the last element of the intrusive list.
   *         Calling this function on an empty container causes undefined behavior.
   * \return Reference to an IntrusiveListNode.
   */
  IntrusiveListNode<T> const& back() const noexcept { return *list_.Prev(); }

  /*!
   * \brief  Remove an element pointed to by the iterator.
   * \param  elem Iterator pointing to the element to be removed from the list.
   * \return An iterator to the next element in the list.
   */
  iterator erase(iterator elem) noexcept {
    iterator next_iterator{elem};
    ++next_iterator;
    elem.GetListNode()->EraseFromList();
    return next_iterator;
  }

  /*!
   * \brief Remove an element given by-value.
   *        Complexity is O(1). The element is removed from whatever intrusive_list it is part of, even if it is not
   * part of this list. \param elem Value to be removed from the list.
   */
  void erase(T& elem) noexcept { elem.EraseFromList(); }

  /*!
   * \brief  Determine whether the list is currently empty.
   * \return True if the list is empty. False if the list has at least one element.
   */
  bool empty() const noexcept { return list_.Next() == &list_; }

  /*!
   * \brief  Iterator to the start of the list.
   * \return The iterator at the start of the list.
   */
  iterator begin() noexcept { return iterator(list_.Next()); }

  /*!
   * \brief  Past-The-End iterator of the list.
   * \return The iterator past-the-end.
   */
  iterator end() noexcept { return iterator(&list_); }

  /*!
   * \brief  Const Iterator to the start of the list.
   * \return The constant iterator at the start of the list.
   */
  const_iterator begin() const noexcept { return const_iterator(list_.Next()); }

  /*!
   * \brief  Const Past-The-End iterator of the list.
   * \return The constant iterator past-the-end.
   */
  const_iterator end() const noexcept { return const_iterator(&list_); }

  /*!
   * \brief  Const Iterator to the start of the list.
   * \return The constant iterator at the start of the list.
   */

  const_iterator cbegin() const noexcept { return begin(); }

  /*!
   * \brief  Const Past-The-End iterator of the list.
   * \return The constant iterator past-the-end.
   */
  const_iterator cend() const noexcept { return end(); }

  /*!
   * \brief  Insert a new element into the list past the element pointed to by the IntrusiveListIterator.
   * \param  where Where to insert.
   * \param  what What to insert.
   * \return An iterator at the location where the new element was inserted.
   */
  iterator insert(iterator where, IntrusiveListNode<T>& what) noexcept {
    where.GetListNode()->InsertAfter(what);
    return iterator(&what);
  }

  /*!
   * \brief Transfer all elements from other into this.
   *        Elements are inserted before pos.
   *        This method is *not* thread-safe.
   * \param pos Element before which elements from other are inserted.
   * \param other Other list from which to obtain elements to insert.
   */
  void splice(iterator pos, IntrusiveList& other) noexcept {
    if (!other.empty()) {
      // VECTOR Next Construct AutosarC++17_10-A7.1.1:
      // Extract elements from other list
      IntrusiveListNode<T>* const other_begin{other.begin().GetListNode()};

      iterator other_last_it{other.end()};
      --other_last_it;

      // VECTOR Next Construct AutosarC++17_10-A7.1.1:
      // Set other list to empty by "erasing" the end element
      // Erasing an element makes its predecessor and successor pointer point to itself,
      // which is the very definition of an empty list.
      IntrusiveListNode<T>* const other_list_node{other.end().GetListNode()};
      other_list_node->EraseFromList();

      // predecessor of pos must have other.begin() as successor
      iterator pred{pos};
      --pred;

      // Splice start of other
      static_cast<void>(pred.GetListNode()->AppendAfter(*other_begin));

      // Splice end of other
      static_cast<void>(other_last_it.GetListNode()->AppendAfter(*pos.GetListNode()));
    }
  }

 private:
  /*!
   * \brief Pointers to the list. next_ points to the head (beginning of the list),
   *        prev_ points to the tail (end of the list).
   */
  IntrusiveListNode<T> list_;
};

// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
/*!
 * \brief Swap contents of two lists.
 * \param left Left list.
 * \param right Right list.
 */
template <typename T>
void swap(IntrusiveList<T>& left, IntrusiveList<T>& right) noexcept {
  IntrusiveList<T> temp;
  temp.splice(temp.end(), left);
  assert(left.empty());
  left.splice(left.end(), right);
  right.splice(right.end(), temp);
}

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_INTRUSIVE_LIST_H_
