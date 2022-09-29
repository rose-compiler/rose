########################################################################
## jsonAstPdf.py
##
## This script along with the AstJSONGeneration module added to
## ROSE replace the older libharu-based AstPDFGeneration module.
## That module traverses an AST and produces a JSON file.  That file
## can then be processed with this script to generate a PDF.  The
## only dependency of this script is the reportlab python package
## which can be installed via:
##
##  pip3 install --user reportlab
##
## (The --user can be excluded if you're installing somewhere other
##  than your user account python package set)
##
## mjs // sept. 2022
##
########################################################################
import argparse
import json
from collections import defaultdict

from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import letter, A4

###
### layout parameters
###
fontName = "Helvetica"
fontSize = 10
xOffset = 5
topOffset = fontSize*2

###
### arguments
###
parser = argparse.ArgumentParser(description="Render JSON tree to PDF")
parser.add_argument('-o', '--output',
                    help="Output file.",
                    required=True)
parser.add_argument('-i', '--input',
                    help="Input JSON file.",
                    required=True)
args = parser.parse_args()

###
### JSON handling.  Read in JSON, calculate tree structure so we can
### set links up and also emit the keys in a reasonable page order
### based on tree traversal
###
with open(args.input, 'r') as f:
    jdata = json.load(f)

# calculate tree structure
nesting = defaultdict(list)
parentmap = {}
childset = set([])
nodeset = set([])
for k in jdata:
    current = k
    parent = jdata[k]["parent"]
    if parent in jdata:
        parentmap[current] = parent

        # store children as ((line,col),key) so we can later sort the
        # children of a node by their source position.
        nesting[parent].append(((jdata[k]["line"],jdata[k]["column"]),current))

        childset.add(current)
        nodeset.add(parent)
    nodeset.add(current)

# roots are all nodes that do not appear as children of any others
roots = nodeset - childset

#
# set up the canvas first so we have something to add the outline to
#
c = canvas.Canvas(args.output, pagesize=letter)
c.setFont(fontName, fontSize)
c.setKeywords("Abstract Syntax Tree (AST) ROSE")
c.setTitle("AST for program code")
c.setAuthor("generic")
c.setCreator("ROSE")
c.setSubject("display of AST for ROSE")
width, height = letter

# we will figure out the page order of keys when we traverse the tree to generate
# the outline
keys_by_pageorder = []

def generate_outline(canvas, node, level=0):
    ninfo = jdata[node]
    if "filename" in ninfo:
        entryText = f'{ninfo["sageClassName"]} {ninfo["filename"]}:{ninfo["line"]}.{ninfo["column"]}'
    else:
        entryText = f'{ninfo["sageClassName"]}'
    keys_by_pageorder.append(node)
    canvas.addOutlineEntry(entryText, node, level, 0)

    # put kids in ascending line/col order, and strip off the (line,col)
    # pair after sorting is done
    kids = [kid[1] for kid in sorted(nesting[node], key=lambda v: v[0])]
    for kid in kids:
        generate_outline(canvas, kid, level+1)

# traverse each rooted tree that was discovered
for root in roots:
    generate_outline(c, root)

# helper function to make a link that optionally does not break a line.
# returns the cursor position after the link is created.
def make_link(canvas, cur, txt, target, breakLine=True):
    textObject = canvas.beginText(cur[0], cur[1])
    textObject.setFillColorRGB(0,0,0.95)
    if breakLine:
        textObject.textLine(txt)
    else:
        textObject.textOut(txt)
    w = canvas.stringWidth(txt)
    canvas.drawText(textObject)
    canvas.linkAbsolute(target, target, Rect=(cur[0], cur[1], cur[0]+w, cur[1]+fontSize), thickness=1)

    cur = textObject.getCursor()
    if breakLine:
        cur = (xOffset, cur[1])

    return cur

# reset the cursor by printing an empty line feed and forcing the x coordinate of
# the cursor to the xOffset parameter
def reset_line(canvas, cur):
    textObject = canvas.beginText(cur[0], cur[1])
    textObject.textLine(text="")
    canvas.drawText(textObject)
    cur = textObject.getCursor()
    cur = (xOffset, cur[1])
    return cur

# start a new page and make sure the font is set up correctly
def new_page(canvas):
    c.showPage()
    c.setFont(fontName, fontSize)

# handle a single page.  these correspond to the keys in the outermost json dictionary.
def handle_page(canvas, key):
    pagedata = jdata[key]
    textObject = canvas.beginText(xOffset, height-topOffset);
    textObject.setFillColorRGB(0.95,0,0)
    textObject.textLine(text=f'pointer: {key}')
    textObject.textLine(text=pagedata["sageClassName"])
    if "filename" in pagedata:
        textObject.textLine(text=f'{pagedata["filename"]} {pagedata["line"]}:{pagedata["column"]}')
    if "isOutputInCodeGeneration" in pagedata:
        textObject.textLine(text=f'isOutputInCodeGeneration: {pagedata["isOutputInCodeGeneration"]}')
    if "isTransformation" in pagedata:
        textObject.textLine(text=f'isTransformation: {pagedata["isTransformation"]}')
    if "declarationMangledName" in pagedata:
        textObject.textLine(text=f'declarationMangledName: {pagedata["declarationMangledName"]}')
    if "symbolName" in pagedata:
        textObject.textLine(text=f'symbolName: {pagedata["symbolName"]}')
    if "expressionType" in pagedata:
        textObject.textLine(text=f'expressionType: {pagedata["expressionType"]}')
    textObject.textLine(text="")
    textObject.textLine(text="")
    canvas.drawText(textObject)
    cur = textObject.getCursor()

    if key in parentmap:
        (startX,startY) = cur
        textObject = canvas.beginText(cur[0], cur[1])
        textObject.setFillColorRGB(0,0,0)
        textObject.textOut("Parent: ")
        canvas.drawText(textObject)
        cur = make_link(canvas, textObject.getCursor(), key, parentmap[key], breakLine=False)
        cur = reset_line(canvas, cur)

    textObject = canvas.beginText(cur[0], cur[1])
    textObject.textLine(text="")

    for entry in pagedata["rti"]:
        textObject.setFillColorRGB(0.6, 0, 0)
        textObject.textOut(entry["type"] + " ")
        textObject.setFillColorRGB(0, 0.6, 0.5)
        textObject.textOut(entry["name"] + ": ")
        textObject.setFillColorRGB(0, 0, 0)
        if len(entry["value"]) > 80:
            textObject.textLine(text="<too long>")
        else:
            if entry["value"] in nodeset:
                canvas.drawText(textObject)
                cur = make_link(canvas, textObject.getCursor(), entry["value"], entry["value"])
                textObject = canvas.beginText(cur[0], cur[1])
            else:
                textObject.textLine(text=entry["value"])
        # check if we're about to fall off the bottom of the page
        if textObject.getCursor()[1] <= fontSize*2:
            canvas.drawText(textObject)
            new_page(canvas)
            textObject = canvas.beginText(xOffset, height-topOffset)

    canvas.drawText(textObject)

    if "attributes" in pagedata:
        cur = textObject.getCursor()
        textObject = canvas.beginText(cur[0], cur[1])
        # check if we're about to fall off the bottom of the page
        # consider a bit more vertical space than usual since we want to fit the
        # section heading as well.
        if textObject.getCursor()[1] <= fontSize*6:
            canvas.drawText(textObject)
            new_page(canvas)
            textObject = canvas.beginText(xOffset, height-topOffset)
        textObject.textLine(text="")
        textObject.setFillColorRGB(0.0, 0.9, 0.1)
        textObject.textLine(text="Attributes:")
        textObject.textLine(text="")
        textObject.setFillColorRGB(0.0, 0.0, 0.0)
        for attribName in pagedata["attributes"]:
            textObject.setFillColorRGB(0.0, 0.6, 0.5)
            textObject.textOut(attribName+": ")
            textObject.setFillColorRGB(0.0, 0.0, 0.0)
            textObject.textLine(pagedata["attributes"][attribName])
            # check if we're about to fall off the bottom of the page
            if textObject.getCursor()[1] <= fontSize*2:
                canvas.drawText(textObject)
                new_page(canvas)
                textObject = canvas.beginText(xOffset, height-topOffset)

        canvas.drawText(textObject)


# iterate over the keys in the order discovered by the tree traversal when the outline was created
for key in keys_by_pageorder:
    c.bookmarkPage(key)
    handle_page(c, key)
    new_page(c)

# save the PDF
c.save()
