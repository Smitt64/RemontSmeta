from mimesis import Person
from mimesis import Address
import sqlite3
from mimesis.enums import Gender

sqlite_connection = sqlite3.connect('E:\\Bruce\\Programming\\RemontSmeta\\rdb.sqlite3')

person = Person('ru')
addr = Address('ru')

def addAdress(partyid):
    cursor = sqlite_connection.cursor()
    cursor.execute('insert into daddress_dbt(t_personid,t_address)values(?,?)', (partyid,addr.address(),))
    cursor.close()

def addContact(partyid, type, value):
    cursor = sqlite_connection.cursor()
    cursor.execute('insert into dcontact_dbt(t_personid,t_type,t_value)values(?,?,?)', (partyid,type,value,))
    cursor.close()

for _ in range(1, 500):
    t_sex = person.sex(iso5218=True)

    _gender = Gender.FEMALE
    if t_sex == 1:
        _gender = Gender.FEMALE

    t_family = person.first_name(_gender)
    t_name = person.name(_gender)
    t_patronymic = person.surname(_gender)

    t_fullname = t_family + ' ' + t_name + ' ' + t_patronymic

    cursor = sqlite_connection.cursor()
    cursor.execute('insert into dperson_dbt(t_family,t_name,t_patronymic,t_fullname,t_sex)values(?,?,?,?,?)', (t_family,t_name,t_patronymic, t_fullname, t_sex,))
    person_id = cursor.lastrowid
    addAdress(person_id)
    addContact(person_id,1,person.telephone())
    addContact(person_id,2,person.email())
    sqlite_connection.commit()
    cursor.close()
