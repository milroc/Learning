//
//  BasicSyntax.h
//  LanguageTricks
//
//  Created by Miles McCrocklin on 10/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


/*THIS CODE IS NOT MEANT TO BE RAN */
@interface BasicSyntax : NSObject
{
@private
    int foo;
}

@property int eye; //create setEye and eye functions
@property (readonly) int eye; //creat eye only.

-(int) eye;
-(void) setEye:anInt; //naming conventions for getter/setter MUST be this way
/*{
    int foo; //protected like fooo
@private
    int bar;
@public 
    double foobar;
@protected
    int fooo; 
}*/

//Names of Methods: foo:bar: or foobar: depends on preference
- (NSArray *) foo:(double)x bar:(int)y; //instance method
+ (NSArray *) bar:(double)x foo:(int)y; //class method 
//(cannot call instance methods)
- foo: (int)x; // returns (id) 
- (void) bar:(int)y; //returns (void) 

+ (void) thisIsAVeryLongMethodName:(int)x 
          withMoreThanOneParameter:(int)y;

- (IBAction) fooo:(id)sender; //(IBAction) same as void
- (IBAction) fooo:sender; //same initialization

@end
