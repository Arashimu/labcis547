; ModuleID = 'sanity1.ll'
source_filename = "sanity1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  call void @__coverage__(i32 2, i32 7)
  call void @llvm.dbg.declare(metadata ptr %x, metadata !15, metadata !DIExpression()), !dbg !16
  call void @__coverage__(i32 2, i32 7)
  store i32 0, ptr %x, align 4, !dbg !16
  call void @__coverage__(i32 3, i32 7)
  call void @llvm.dbg.declare(metadata ptr %y, metadata !17, metadata !DIExpression()), !dbg !18
  call void @__coverage__(i32 3, i32 11)
  %0 = load i32, ptr %x, align 4, !dbg !19
  call void @__coverage__(i32 3, i32 7)
  store i32 %0, ptr %y, align 4, !dbg !18
  call void @__coverage__(i32 4, i32 7)
  call void @llvm.dbg.declare(metadata ptr %z, metadata !20, metadata !DIExpression()), !dbg !21
  call void @__coverage__(i32 4, i32 11)
  %1 = load i32, ptr %y, align 4, !dbg !22
  call void @__coverage__(i32 4, i32 15)
  %2 = load i32, ptr %x, align 4, !dbg !23
  call void @__sanitize__(i32 %2, i32 4, i32 13)
  call void @__coverage__(i32 4, i32 13)
  %div = sdiv i32 %1, %2, !dbg !24
  call void @__coverage__(i32 4, i32 7)
  store i32 %div, ptr %z, align 4, !dbg !21
  call void @__coverage__(i32 5, i32 3)
  ret i32 0, !dbg !25
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @__coverage__(i32, i32)

declare void @__sanitize__(i32, i32, i32)

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Ubuntu clang version 15.0.7", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "sanity1.c", directory: "/home/srh/cis547/lab4/test", checksumkind: CSK_MD5, checksum: "6431976a10f88bf41dcae72859f43e93")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Ubuntu clang version 15.0.7"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 1, type: !11, scopeLine: 1, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "x", scope: !10, file: !1, line: 2, type: !13)
!16 = !DILocation(line: 2, column: 7, scope: !10)
!17 = !DILocalVariable(name: "y", scope: !10, file: !1, line: 3, type: !13)
!18 = !DILocation(line: 3, column: 7, scope: !10)
!19 = !DILocation(line: 3, column: 11, scope: !10)
!20 = !DILocalVariable(name: "z", scope: !10, file: !1, line: 4, type: !13)
!21 = !DILocation(line: 4, column: 7, scope: !10)
!22 = !DILocation(line: 4, column: 11, scope: !10)
!23 = !DILocation(line: 4, column: 15, scope: !10)
!24 = !DILocation(line: 4, column: 13, scope: !10)
!25 = !DILocation(line: 5, column: 3, scope: !10)
