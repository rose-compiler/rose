#! /usr/apps/python/2.4.3/bin/python

from Relations import NodeRelations, ListRelations
from Visitor import *
from string import Template


def buildInitAndDecls( nodes, lists ):
    constructor=''
    relation_decls=''
    seen={}
    for r in nodes:
        if r['relation'] in seen:
            continue
        else:
            seen[ str(r['relation']) ]= True;

        relation_decls += "    Relation *%s;\n"%(r['relation'])
        constructor += NodeInit.substitute(
                relation= r['relation'],
                arity= r['arity'],
                set_doc= r['set_doc']
            )

    for l in lists:
        relation_decls += "    Relation *%s;\n"%(l['first_relation'])
        relation_decls += "    Relation *%s;\n"%(l['next_relation'])
        constructor += ListInit.substitute(
                first_relation= l['first_relation'],
                next_relation= l['next_relation']
            )

    return ( constructor, relation_decls )


def buildVisit( nodes, lists ):
    visit=''

    for r in nodes:
        for type in r['types']:
            if 'pred' in r and r['pred'] != None:
                pred= ' && (%s) '%r['pred']
            else:
                pred= ''
            visit += NodeVisit.substitute(
                    type= type,
                    arity= r['arity'],
                    relation= r['relation'],
                    set_tuple= r['set_tuple'],
                    pred= pred
                )

    for l in lists:
        if 'pred' in r and r['pred'] != None:
            pred= ' && (%s) '%r['pred']
        else:
            pred= ''
        l['pred']= pred
        visit += ListVisit.substitute( l )
    return visit


init,decls= buildInitAndDecls( NodeRelations, ListRelations )
visit= buildVisit( NodeRelations, ListRelations )

header= open('output/RelationVisitor.hh','w')
header.write(Header.substitute( private_decls= decls ))
header.close()

source= open('output/RelationVisitor.cc','w')
source.write(Source.substitute( visit=visit, constructor=init ))
source.close()

