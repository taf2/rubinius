; RUN: llvm-as < %s | opt -simplifycfg | llvm-dis | not grep br

define i32 @test1(i1 %C) {
entry:
        br i1 %C, label %T, label %F
T:              ; preds = %entry
        ret i32 1
F:              ; preds = %entry
        ret i32 0
}

define void @test2(i1 %C) {
        br i1 %C, label %T, label %F
T:              ; preds = %0
        ret void
F:              ; preds = %0
        ret void
}

