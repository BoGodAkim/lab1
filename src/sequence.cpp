#include "sequence.hpp"

namespace CHECK
{
    struct No
    {
    };
    template <typename T, typename Arg>
    No operator==(const T &, const Arg &);

    template <typename T, typename Arg = T>
    struct EqualExists
    {
        enum
        {
            value = !std::is_same<decltype(*(T *)(0) == *(Arg *)(0)), No>::value
        };
    };
}

template <typename Key, typename Info>
Sequence<Key, Info>::Node::Node(Key key, Info value, Node *next)
{
    this->key = key;
    this->value = value;
    this->next = next;
}

template <typename Key, typename Info>
void Sequence<Key, Info>::checkTemplateOnOperators() const
{
    if (!CHECK::EqualExists<Key, Key>::value)
    {
        throw "Key does not have operator==";
    }
    if (!CHECK::EqualExists<Info, Info>::value)
    {
        throw "Info does not have operator==";
    }
}

template <typename Key, typename Info>
Sequence<Key, Info>::Sequence()
{
    checkTemplateOnOperators();
    head = nullptr;
    tail = nullptr;
    length = 0;
}

template <typename Key, typename Info>
Sequence<Key, Info>::Sequence(const Sequence<Key, Info> &other)
{
    checkTemplateOnOperators();
    head = nullptr;
    tail = nullptr;
    length = 0;
    Node *current = other.head;
    while (current != nullptr)
    {
        insert(current->key, current->value);
        current = current->next;
    }
}

template <typename Key, typename Info>
Sequence<Key, Info>::~Sequence()
{
    Node *current = head;
    while (current != nullptr)
    {
        Node *temp = current;
        current = current->next;
        delete temp;
    }
}

template <typename Key, typename Info>
typename std::pair<typename Sequence<Key, Info>::Node *, typename Sequence<Key, Info>::Node *>
Sequence<Key, Info>::find(const Key &key, int occurrence) const
{
    if (occurrence < 0)
    {
        throw "Occurrence must be non-negative";
    }
    Node *current = head;
    Node *previous = nullptr;
    int count = 0;
    while (current != nullptr)
    {
        if (current->key == key)
        {
            if (count == occurrence)
            {
                return make_pair(previous, current);
            }
            count++;
        }
        previous = current;
        current = current->next;
    }
    return make_pair(nullptr, nullptr);
}

template <typename Key, typename Info>
void Sequence<Key, Info>::insert(const Key &key, const Info &value, bool atEnd)
{
    if (head == nullptr)
    {
        head = new Node(key, value, nullptr);
        tail = head;
        length++;
        return;
    }

    if (atEnd)
    {
        tail->next = new Node(key, value, nullptr);
        tail = tail->next;
    }
    else
    {
        Node *temp = head;
        head = new Node(key, value, temp);
    }
    length++;
}

template <typename Key, typename Info>
void Sequence<Key, Info>::insert(const Key &key, const Info &value, const Key &after, int occurrence)
{
    Node *current = find(after, occurrence).second;
    if (current == nullptr)
    {
        throw "Key not found";
    }
    else
    {
        Node *temp = current->next;
        current->next = new Node(key, value, temp);
        length++;
        if (temp == nullptr)
        {
            tail = current->next;
        }
    }
}

template <typename Key, typename Info>
Info &Sequence<Key, Info>::operator[](const Key &key) const
{
    Node *current = find(key).second;
    if (current == nullptr)
    {
        throw "Key not found";
    }
    else
    {
        return current->value;
    }
}

template <typename Key, typename Info>
Info &Sequence<Key, Info>::operator()(const Key &key, int occurrence) const
{
    Node *current = find(key, occurrence).second;
    if (current == nullptr)
    {
        throw "Key not found";
    }
    else
    {
        return current->value;
    }
}

template <typename Key, typename Info>
bool Sequence<Key, Info>::operator==(const Sequence<Key, Info> &other) const
{
    if (length != other.length)
    {
        return false;
    }
    Node *current = head;
    Node *otherCurrent = other.head;
    while (current != nullptr)
    {
        if (!(current->key == otherCurrent->key) || !(current->value == otherCurrent->value))
        {
            return false;
        }
        current = current->next;
        otherCurrent = otherCurrent->next;
    }
    return true;
}

template <typename Key, typename Info>
void Sequence<Key, Info>::remove(const Key &key, int occurrence)
{
    pair<Node *, Node *> result = find(key, occurrence);
    Node *previous = result.first;
    Node *current = result.second;

    if (current == nullptr)
    {
        throw "Key not found";
    }
    else
    {
        if (previous == nullptr)
        {
            head = current->next;
        }
        else
        {
            previous->next = current->next;
        }

        if (current == tail)
        {
            tail = previous;
        }
        delete current;
        length--;
    }
}

template <typename Key, typename Info>
int Sequence<Key, Info>::occurrences(const Key &key) const
{
    Node *current = head;
    int count = 0;
    while (current != nullptr)
    {
        if (current->key == key)
        {
            count++;
        }
        current = current->next;
    }
    return count;
}

template <typename Key, typename Info>
bool Sequence<Key, Info>::has(const Key &key) const
{
    Node *current = head;
    while (current != nullptr)
    {
        if (current->key == key)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

template <typename Key, typename Info>
int Sequence<Key, Info>::size() const
{
    return length;
}

template <typename Key, typename Info>
typename Sequence<Key, Info>::Iterator Sequence<Key, Info>::begin() const
{
    return Iterator(head);
}

template <typename Key, typename Info>
typename Sequence<Key, Info>::Iterator Sequence<Key, Info>::end() const
{
    return Iterator(tail);
}

template <typename Key, typename Info>
Sequence<Key, Info>::Iterator::Iterator(Node *node)
{
    current = node;
}

template <typename Key, typename Info>
Sequence<Key, Info>::Iterator::Iterator(Iterator &other)
{
    current = other.current;
}

template <typename Key, typename Info>
typename Sequence<Key, Info>::Iterator &Sequence<Key, Info>::Iterator::operator++()
{
    if (current->next == nullptr)
    {
        throw "End of sequence";
    }
    else
    {
        current = current->next;
        return *this;
    }
}

template <typename Key, typename Info>
typename Sequence<Key, Info>::Iterator &Sequence<Key, Info>::Iterator::operator++(int)
{
    if (current->next == nullptr)
    {
        throw "End of sequence";
    }
    else
    {
        Iterator *temp = new Iterator(*this);
        current = current->next;
        return *temp;
    }
}

template <typename Key, typename Info>
bool Sequence<Key, Info>::Iterator::operator==(const Iterator &other) const
{
    return current == other.current;
}

template <typename Key, typename Info>
bool Sequence<Key, Info>::Iterator::operator!=(const Iterator &other) const
{
    return current != other.current;
}

template <typename Key, typename Info>
void Sequence<Key, Info>::Iterator::operator=(const Iterator &other)
{
    current = other.current;
}

template <typename Key, typename Info>
std::pair<Key, Info> Sequence<Key, Info>::Iterator::operator*() const
{
    return make_pair(current->key, current->value);
}

template <typename Key, typename Info>
bool Sequence<Key, Info>::Iterator::isLast() const
{
    return current->next == nullptr;
}