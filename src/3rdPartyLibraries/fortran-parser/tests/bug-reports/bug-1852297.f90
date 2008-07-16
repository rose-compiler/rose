!
! Bug 1852297 on sf.net, Label action called twice in label_list (R313)
!
integer computed_goto_selector
10 continue
GO TO ( 10, 20, 30 ) computed_goto_selector
20 continue
30 continue
end
