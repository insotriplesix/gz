
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;
import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class Main {

    public static void main(String[] args) {
        JAXBContext jc = null;
        try {

            XMLInputFactory xmlInputFactory = XMLInputFactory.newInstance();
            XMLStreamReader streamReader = xmlInputFactory.createXMLStreamReader(new FileReader("res/test.xml"));

            jc = JAXBContext.newInstance(Data1.class, Data2.class);
            Unmarshaller unmarshaller = jc.createUnmarshaller();

            List<ClientData> list = new ArrayList<>();

            while (streamReader.hasNext()) {
                streamReader.next();
                if (streamReader.isStartElement() && streamReader.getLocalName().contains("старт"))
                {
                    if (streamReader.hasNext()) {
                        ClientData cli = new ClientData();
                        streamReader.next();
                        parseJopa(streamReader, cli);
                        Data data = (Data) unmarshaller.unmarshal(streamReader);
                        cli.data = data;
                        list.add(cli);
                    }
                }

            }

            for (ClientData cd : list) {
                System.out.println(cd.data);
            }
        } catch (JAXBException | FileNotFoundException | XMLStreamException e) {
            e.printStackTrace();
        }
    }

    public static void parseJopa(XMLStreamReader streamReader, ClientData cli) throws XMLStreamException {
        while (streamReader.hasNext()) {
            streamReader.next();
            if (streamReader.isStartElement() && streamReader.getLocalName().equals("хуйня"))
            {
                cli.hujna = streamReader.getElementText();
            }
            else if (streamReader.isStartElement() && streamReader.getLocalName().equals("типкли"))
            {
                cli.cliType = Integer.parseInt(streamReader.getElementText());
            }
            else if (streamReader.isStartElement() && streamReader.getLocalName().equals("юл")
                    || (streamReader.isStartElement() && streamReader.getLocalName().equals("физ"))) {
                break;
            }
        }
    }
}
