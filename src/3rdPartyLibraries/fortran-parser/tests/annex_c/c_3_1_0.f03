! this is the first example in C.3.1
type id_numbers
   integer ssn
   integer employee_number
end type id_numbers

type person_id
   character(len=30) last_name
   character(len=1) middle_initial
   character(len=30) first_name
   type(id_numbers) number
end type person_id

type person
   integer age
   type(person_id) id
end type person

type (person) george, mary

print *, george%age

print *, mary%id%last_name
print *, mary%id%number%ssn
print *, george%id%number

end
