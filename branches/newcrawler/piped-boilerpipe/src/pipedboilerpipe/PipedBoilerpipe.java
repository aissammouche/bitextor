/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package pipedboilerpipe;

import de.l3s.boilerpipe.BoilerpipeProcessingException;
import de.l3s.boilerpipe.document.TextDocument;
import de.l3s.boilerpipe.extractors.ArticleExtractor;
import de.l3s.boilerpipe.sax.BoilerpipeSAXInput;
import de.l3s.boilerpipe.sax.HTMLHighlighter;
import java.io.StringReader;
import java.util.Scanner;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * Java tool that reads HTML documents from STDIN (one per line) and applies
 * the ArticleExtractor in BoilerPipe to clean it. Java tool that reads HTML
 * documents from STDIN (one per line) and applies the ArticleExtractor in
 * BoilerPipe to clean it
 */
public class PipedBoilerpipe {
    /**
     * Java tool that reads HTML documents from STDIN (one per line) and applies
     * the ArticleExtractor in BoilerPipe to clean it
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Scanner stdin = new Scanner(System.in);
        while(stdin.hasNextLine())
        {
            try {
                String[] fields=stdin.nextLine().split("\t");
                if(fields.length==6){
                    //Reading a line
                    String line=fields[5];
                    //Processing XHTML
                    StringReader reader = new StringReader(line);
                    TextDocument source = new BoilerpipeSAXInput(new InputSource(reader)).getTextDocument();
                    //Processing XHTML to remove boilerplates
                    ArticleExtractor extractor=ArticleExtractor.INSTANCE;
                    extractor.process(source);
                    //Producing clean XHTML
                    HTMLHighlighter h=HTMLHighlighter.newExtractingInstance();
                    fields[5]=h.process(source, line).replace("\n", " ");
                    StringBuilder sb=new StringBuilder();
                    for(String f: fields){
                        sb.append(f);
                        sb.append("\t");
                    }
                    sb.deleteCharAt(sb.length()-1);
                    System.out.println(sb.toString());
                }
            } catch (SAXException ex) {
                ex.printStackTrace(System.err);
            } catch (BoilerpipeProcessingException ex){
                ex.printStackTrace(System.err);
            }
        }
    }
}
