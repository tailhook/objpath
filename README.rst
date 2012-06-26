=======
Objpath
=======

Objpath is a library that allows to traverse data structures by path. It can
be used to build fast traversers for JSON and JSON-like data structures (YAML,
msgpack). Potentially may be used to traverse any kind of data structures
which has two kinds of access: based on the key (objects in JSON) and index
(arrays in JSON).


Syntax
======

``/key`` access an object property with name ``key`` (allows only alphanumeric
characters)

``:123`` access an array element with index ``123``

``/*`` traverse all properties of an object

``:*`` traverse all values of an array

``/%`` traverse all keys of the object (traversing keys is usually an
operation that returns the keys itself, its useless to access children of the
key)

``/abc*`` traverse values with keys starting with ``abc``

``:12-34`` traverse all values from 12 up to 34 (including both)

``["abc"]`` access an object property with naem ``abc`` (allows
non-alphanumeric characters)

``A?B`` means that we check whether pattern B matches but return whatever the A
matches

To allow easier concatenation of the patterns, two or more slash characters
are squashed and treated as there was only one.


API
===

The API is designed in a way that adding additional syntax to patterns should
not change the underlying API. However, some optional additions to API may be,
to allow more efficient search. Also the API should be very easy to use from
scripting languages.


