

! Testing dealloc opt list.
deallocate (this, stat=test)
deallocate (that, stat=test, errmsg=whatever)
end
