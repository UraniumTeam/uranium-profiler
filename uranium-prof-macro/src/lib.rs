#[cfg(test)]
mod tests;

use quote::quote;
use proc_macro2::{TokenStream, TokenTree, Literal};
use syn::{Item, ItemFn, parse_macro_input, parse_quote};

/**
Function like this:
```
    fn foo() -> T { /* code here */ }
```
Is transformed to:
```
    fn foo() -> T {
        static _FN_DATA: FunctionData = FunctionData{ name: "foo()" };
        prof_func! {
            /* code here */
        }
    }
```
 */
#[proc_macro_attribute]
pub fn profile_func(_attr: proc_macro::TokenStream, item: proc_macro::TokenStream)
                    -> proc_macro::TokenStream {
    if let Some(x) = item.clone().into_iter().last() {
        if let proc_macro::TokenTree::Punct(_) = x {
            return item
        }
    }
    let mut function = parse_macro_input!(item as ItemFn);
    let name = function.clone().sig.ident.to_string();

    let body = &function.block;
    if function.sig.abi.is_none() {
        let new_body: syn::Block = parse_quote! {
            {
                static _FN_DATA: FunctionData = FunctionData{ name: #name };
                prof_func! { #body }
            }
        };

        function.block = Box::new(new_body);
    }
    else {
        panic!("wjeciojwecmkwqopvheuhfo")
    }

    (quote! { #function }).into()
}
