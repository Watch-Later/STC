# STC [cbox](../include/stc/cbox.h): Smart Pointer (Boxed object)

**cbox** is a A box is a smart pointer to a heap allocated value of type X. A **cbox** can
be empty. The *cbox_X_cmp()*, *cbox_X_drop()* methods are defined based on the `i_cmp`
and `i_valdrop` macros specified. Use *cbox_X_clone(p)* to make a deep copy, which uses the
`i_valfrom` macro if defined.

When declaring a container of **cbox** values, it is recommended to define `i_val_bind` to the
cbox type instead of defining `i_val`. This will auto-set `i_drop`, `i_from`, and `i_cmp` using 
functions defined by the specified **cbox**.

For containers, make sure to pass the result of create functions like *cbox_X_new()* **only** to 
*insert()*, *push_back()*, and *push()* functions. Use the *emplace()* functions in order to
auto-*clone* an already existing/owned cbox element.

See similar c++ class [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) for a functional reference, or Rust [std::boxed::Box](https://doc.rust-lang.org/std/boxed/struct.Box.html)

## Header file and declaration

```c
#define i_val           // value: REQUIRED
#define i_cmp           // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_drop          // destroy value func - defaults to empty destruct
#define i_from          // create from raw/clone func - REQUIRED if i_drop is defined,
                        // unless 'i_opt c_no_clone' is defined.
#define i_tag           // type name tag, defaults to i_val
#include <stc/cbox.h>    
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.
Define `i_opt` with `c_no_cmp` if comparison between i_val's is not needed/available. Will then
compare the pointer addresses when used. Additionally, `c_no_clone` or `i_is_fwd` may be defined.

## Methods
```c
cbox_X      cbox_X_init();                                    // return an empty cbox
cbox_X      cbox_X_new(i_valraw raw);                         // like cbox_X_from(), but create owned value from raw.
cbox_X      cbox_X_from(i_val val);                           // allocate new heap object with val. Take ownership of val.
cbox_X      cbox_X_from_ptr(i_val* p);                        // create a cbox from a pointer. Takes ownership of p.

cbox_X      cbox_X_clone(cbox_X other);                       // return deep copied clone
cbox_X      cbox_X_move(cbox_X* self);                        // transfer ownership to another cbox.
void        cbox_X_take(cbox_X* self, cbox_X other);          // take ownership of other.
void        cbox_X_copy(cbox_X* self, cbox_X other);          // deep copy to self
void        cbox_X_drop(cbox_X* self);                         // destruct the contained object and free's it.

void        cbox_X_reset(cbox_X* self);   
void        cbox_X_reset_from(cbox_X* self, i_val val);       // assign new cbox with value. Takes ownership of val.

uint64_t    cbox_X_value_hash(const i_val* x, size_t n);      // hash value
int         cbox_X_value_cmp(const i_val* x, const i_val* y); // compares pointer addresses if 'i_opt c_no_cmp'
                                                              // is defined. Otherwise uses 'i_cmp' or default compare.
bool        cbox_X_value_eq(const i_val* x, const i_val* y);  // cbox_X_value_cmp == 0
```
## Types and constants

| Type name          | Type definition                                               | Used to represent...     |
|:-------------------|:--------------------------------|:------------------------|
| `cbox_null`        | `{NULL}`                        | Init nullptr const      |
| `cbox_X`           | `struct { cbox_X_value* get; }` | The cbox type           |
| `cbox_X_value`     | `i_val`                         | The cbox element type   |

## Example

```c
#include <stdio.h>
#include <string.h>

void int_drop(int* x) {
    printf("drop: %d\n", *x);
}

// When 'i_drop' is defined, you are also forced to define a clone function with
// 'i_from', as it is normally required when i_drop destroys resources.
//
// If cloning is not needed, define 'i_opt c_no_clone' instead of 'i_from'
// both for the cbox type and the container of cbox elements. It will also 
// disable emplace container functions.
//
// This applies to all container types, except those with carc elements, as they
// define cloning internally.

#define i_val int
#define i_drop int_drop       // optional func, just to display elements destroyed
#define i_from c_default_clone
#include <stc/cbox.h>         // cbox_int

#define i_key_sptr cbox_int   // note: use i_key_sptr instead of i_key
#define i_tag int             // tag otherwise defaults to 'ref'
#include <stc/csset.h>        // csset_int (like: std::set<std::unique_ptr<int>>)

#define i_val_sptr cbox_int   // note: use i_val_sptr instead of i_val
#define i_tag int             // tag otherwise defaults to 'ref'
#include <stc/cvec.h>         // cvec_int (like: std::vector<std::unique_ptr<int>>)

int main()
{
    c_auto (cvec_int, vec)   // declare and init vec, call drop at scope exit
    c_auto (csset_int, set)  // declare and init set, call drop at scope exit
    {
        c_apply(v, cvec_int_push_back(&vec, v), cbox_int, {
            cbox_int_new(2021),
            cbox_int_new(2012),
            cbox_int_new(2022),
            cbox_int_new(2015),
        });
        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);
        puts("");

        // add odd numbers from vec to set
        c_foreach (i, cvec_int, vec)
            if (*i.ref->get & 1)
                csset_int_emplace(&set, *i.ref); // deep copy (clones) *i.ref object

        // erase the two last elements in vec
        cvec_int_pop_back(&vec);
        cvec_int_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_int, set) printf(" %d", *i.ref->get);

        puts("\nDone");
    }
}
```
Output:
```
vec: 2021 2012 2022 2015
drop: 2015
drop: 2022
vec: 2021 2012
set: 2015 2021
Done
drop: 2021
drop: 2015
drop: 2021
drop: 2012
```